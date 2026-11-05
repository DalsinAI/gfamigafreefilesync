// process_priority_amiga.cpp - zen/process_priority.h for AmigaOS, in
// place of zen/process_priority.cpp (Linux: ioprio syscalls). "Background"
// runs FreeFileSync's task at priority -1, below the Workbench and the
// Shells, and puts the old priority back afterwards.
//
// Copyright (c) 2026 AmiCradle Team. GPL 3, like FreeFileSync.
#include <zen/process_priority.h>
#include <proto/exec.h>

using namespace zen;

struct SetProcessPriority::Impl
{
    struct Task* task = nullptr;
    BYTE oldPri = 0;
};

SetProcessPriority::SetProcessPriority(ProcessPriority prio) : //throw FileError
    pimpl_(new Impl)
{
    if (prio == ProcessPriority::background)
    {
        pimpl_->task = FindTask(nullptr);
        pimpl_->oldPri = SetTaskPri(pimpl_->task, -1);
    }
}

SetProcessPriority::~SetProcessPriority()
{
    if (pimpl_->task)
        SetTaskPri(pimpl_->task, pimpl_->oldPri);
}
