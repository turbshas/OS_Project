#include "process.h"

static uint32_t processCounter = ROOT_PROCESS_ID;
static uint32_t
getNextProcessId()
{
    processCounter++;
    return processCounter;
}

Process::Process()
    : _parentProcessId(0),
      _processId(0),
      _memMgr(nullptr),
      _state(ProcessState::Dead),
      _swapped(false),
      _returnCode(0),
      _mainThread(),
      _memRegionList(),
      _threadList()
{
}

Process::Process(const uint32_t parentProcessId, MemoryManager* const memMgr, const VoidFunction startAddress)
    : _parentProcessId(parentProcessId),
      _processId(getNextProcessId()),
      _memMgr(memMgr),
      _state(ProcessState::Created),
      _swapped(false),
      _returnCode(0),
      _mainThread(*this, memMgr),
      _memRegionList(),
      _threadList()
{
    _mainThread.SetEntryPoint(startAddress);
}

Process::Process(const Process& other)
    : _parentProcessId(other._parentProcessId),
      _processId(other._processId),
      _memMgr(other._memMgr),
      _state(other._state),
      _swapped(other._swapped),
      _returnCode(other._returnCode),
      _mainThread(other._mainThread),
      _memRegionList(other._memRegionList),
      _threadList(other._threadList)
{
}

Process::Process(Process&& source)
    : Process()
{
    *this = static_cast<Process&&>(source);
}

Process::~Process()
{
    // Empty thread list and call destructor on each thread since threads are stored by value so won't get called otherwise.
    // I think this is fine since it's a copy of the original?
    while (!_threadList.empty())
    {
        Thread thread = _threadList.popFront();
        thread.~Thread();
    }
    while (_memRegionList.empty())
    {
        const MemRegion memRegion = _memRegionList.popFront();
        _memMgr->Free(memRegion);
    }
}

Process&
Process::operator=(const Process& other)
{
    if (&other == this) return *this;

    _parentProcessId = other._parentProcessId;
    _processId = other._processId;
    _memMgr = other._memMgr;
    _state = other._state;
    _swapped = other._swapped;
    _returnCode = other._returnCode;
    _memRegionList = other._memRegionList;
    _threadList = other._threadList;

    return *this;
}

Process&
Process::operator=(Process&& other)
{
    if (&other == this) return *this;

    _parentProcessId = other._parentProcessId;
    other._parentProcessId = 0;

    _processId = other._processId;
    other._processId = 0;

    _memMgr = other._memMgr;
    other._memMgr = nullptr;

    _state = other._state;
    other._state = ProcessState::Dead;

    _swapped = other._swapped;
    other._swapped = false;

    _returnCode = other._returnCode;
    other._returnCode = 0;

    _memRegionList = other._memRegionList;
    other._memRegionList.clear();

    _threadList = other._threadList;
    other._threadList.clear();

    return *this;
}

void*
Process::AllocateMemory(const size_t numBytes)
{
    const MemRegion memRegion = _memMgr->Allocate(numBytes);
    if (memRegion.start() == 0) return nullptr;
    AddMemRegion(memRegion);
    return reinterpret_cast<void*>(memRegion.start());
}

void
Process::AddMemRegion(const MemRegion& memRegion)
{
    _memRegionList.pushBack(memRegion);
}
