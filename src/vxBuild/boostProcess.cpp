#define BOOST_ALLOW_DEPRECATED_HEADERS
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/process.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/asio.hpp>

#include "../../extern/tiny-process-library/process.hpp"

#include <algorithm>
#include <string>
#include <vector>
#include <queue>
#include <iostream>
#include <fstream>

namespace fs = boost::filesystem;
namespace bp = boost::process;
namespace ba = boost::asio;
namespace bs = boost::system;

#define StdPipeFunction std::function<void(const bs::error_code & ec, std::size_t n)>

struct Process;

typedef struct BPProcess 
{
    Process* rpProcess;

    bp::child bpChildProcess;
    ba::io_service ios;

    StdPipeFunction onStdOutFunction;
    char stdOutBufferVector[128 << 10];
    ba::mutable_buffers_1 stdOutBuffer;
    bp::async_pipe stdOutPipe;

    StdPipeFunction onStdErrorFunction;
    char stdErrorBufferVector[128 << 10];
    ba::mutable_buffers_1 stdErrorBuffer;
    bp::async_pipe stdErrorPipe;

    BPProcess(Process * process);
    ~BPProcess();
} BPProcess;


BPProcess::BPProcess(Process * process)
    : stdOutBuffer(ba::buffer(stdOutBufferVector))
    , stdErrorBuffer(ba::buffer(stdErrorBufferVector))
    , stdOutPipe(bp::async_pipe(ios))
    , stdErrorPipe(bp::async_pipe(ios))
{
    rpProcess = process;
    onStdOutFunction = [&](const bs::error_code & ec, size_t n)
    {
        rpProcess->stdOutStream.write(stdOutBufferVector, n);
        if (!ec)
        {
            ba::async_read(stdOutPipe, stdOutBuffer, onStdOutFunction);
        }
    };

    onStdErrorFunction = [&](const bs::error_code & ec, size_t n)
    {
        rpProcess->stdErrorStream.write(stdErrorBufferVector, n);
        if (!ec)
        {
            ba::async_read(stdErrorPipe, stdErrorBuffer, onStdErrorFunction);
        }
    };

    bpChildProcess = bp::child(bp::exe(rpProcess->exePath),bp::args(rpProcess->args),bp::std_out > stdOutPipe,bp::std_err > stdErrorPipe);

    ba::async_read(stdOutPipe, stdOutBuffer, onStdOutFunction);
    ba::async_read(stdErrorPipe, stdErrorBuffer, onStdErrorFunction);

    ios.run();
}

BPProcess::~BPProcess()
{
    if (bpChildProcess.valid())
    {
        bpChildProcess.wait();            
        bpChildProcess.terminate();
    }
    if (!ios.stopped())
    {
        ios.stop();
    }
    stdOutPipe.close();
    stdErrorPipe.close();
}
