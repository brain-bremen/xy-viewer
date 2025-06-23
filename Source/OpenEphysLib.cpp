/*
    ------------------------------------------------------------------

    This file is part of the XY Viewer Plugin for the Open Ephys GUI
    Copyright (C) 2016 Open Ephys
    Copyright (C) 2025 Joscha Schmiedt <schmiedt@uni-bremen.de>

    ------------------------------------------------------------------

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <PluginInfo.h>

#include "XYViewer.h"

#ifdef WIN32
#include <Windows.h>
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute__((visibility("default")))
#endif

using namespace Plugin;

constexpr auto NUM_PLUGINS = 1;

extern "C" EXPORT void getLibInfo(Plugin::LibraryInfo* info)
{
    info->apiVersion = PLUGIN_API_VER;
    info->name = "XY Viewer";   // Name of the plugin library
    info->libVersion = "0.1.0"; // Version of the plugin
    info->numPlugins = NUM_PLUGINS;
}

extern "C" EXPORT int getPluginInfo(int index, Plugin::PluginInfo* info)
{
    switch (index)
    {
        // one case per plugin. This example is for a processor which connects directly to the
        // signal chain
    case 0:

        // Type of plugin. See "Source/Processors/PluginManager/OpenEphysPlugin.h" for complete info
        // about the different type structures
        info->type = Plugin::Type::PROCESSOR;

        // Processor name
        info->processor.name = "XY Viewer"; // Processor name shown in the GUI

        // Type of processor. Visualizers are usually sinks, but they can also be SOURCE or FILTER
        // processors.
        info->processor.type = Processor::Type::SINK;

        // Class factory pointer. Replace "ProcessorPluginSpace::ProcessorPlugin" with the namespace
        // and class name.
        info->processor.creator = &(Plugin::createProcessor<XYViewerPlugin::XYViewer>);
        break;

    default:
        return -1;
        break;
    }
    return 0;
}

#ifdef WIN32
BOOL WINAPI DllMain(IN HINSTANCE hDllHandle, IN DWORD nReason, IN LPVOID Reserved)
{
    return TRUE;
}

#endif
