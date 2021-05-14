/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 6.0.7

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "TextEditorAttachment.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class GuiComponent  : public juce::Component,
                      public juce::TextEditor::Listener
{
public:
    //==============================================================================
    GuiComponent (WIDERAudioProcessor& p);
    ~GuiComponent() override;

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.

    void textEditorTextChanged(juce::TextEditor& t) override{}
    void textEditorReturnKeyPressed(juce::TextEditor& t) override;
    void textEditorEscapeKeyPressed(juce::TextEditor& t) override{}
    void textEditorFocusLost(juce::TextEditor& t) override;
    void onChangeTextEdit(juce::TextEditor& t);
    //[/UserMethods]

    void paint (juce::Graphics& g) override;
    void resized() override;

    // Binary resources:
    static const char* bg_png;
    static const int bg_pngSize;


private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    WIDERAudioProcessor& processor;
    juce::AudioProcessorValueTreeState& valueTreeState;

    OpenGLContext openGLContext;
    
    std::unique_ptr<TextEditorAttachment> textEditorAttachment;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<juce::TextEditor> wideTextEditor;
    juce::Image cachedImage_bg_png_1;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuiComponent)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

