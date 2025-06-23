/*
    ------------------------------------------------------------------

    This file is part of the XY Viewer Plugin for the Open Ephys GUI
    Copyright (C) 2022 Open Ephys
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

#ifndef XYVIEWEREDITOR_H_DEFINED
#define XYVIEWEREDITOR_H_DEFINED

#include <VisualizerEditorHeaders.h>

namespace XYViewerPlugin
{

class XYViewerEditor : public VisualizerEditor, public ComboBox::Listener
{
public:
    XYViewerEditor (GenericProcessor* parentNode);
    ~XYViewerEditor() override = default;

    Visualizer* createNewCanvas() override;

    void comboBoxChanged (ComboBox* comboBoxThatHasChanged) override;
    void selectedStreamHasChanged() override;

private:
    std::unique_ptr<ComboBox> xChannelList = nullptr;
    std::unique_ptr<ComboBox> yChannelList = nullptr;

    /** Generates an assertion if this class leaks */
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XYViewerEditor);
};

} // namespace XYViewerPlugin
#endif // XYVIEWEREDITOR_H_DEFINED