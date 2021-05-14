/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GuiComponent.h"
#include "OpenGLComponent.h"

//==============================================================================
/**
*/
class WIDERAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    WIDERAudioProcessorEditor (WIDERAudioProcessor&);
    ~WIDERAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
private:    
    WIDERAudioProcessor& audioProcessor;
    GuiComponent guiComponent;
    
    // shader
    OpenGLComponent openGLComponent;
    std::unique_ptr<OpenGLComponentAttachment> openGLComponentAttachment;
//    OpenGLContext openGLContext;
    
    AudioProcessorValueTreeState& valueTreeState;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WIDERAudioProcessorEditor)
};
