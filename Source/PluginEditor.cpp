/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WIDERAudioProcessorEditor::WIDERAudioProcessorEditor (WIDERAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(p.parameters), guiComponent(p)
{
    setOpaque (true);
    if (auto* peer = getPeer()) peer->setCurrentRenderingEngine (0); // peer = manager of OS window
    addAndMakeVisible (guiComponent);

    addAndMakeVisible (openGLComponent);
    openGLComponent.setBounds (26, 158, 288, 48);
    openGLComponentAttachment.reset(new OpenGLComponentAttachment(valueTreeState, "WIDE", openGLComponent));
    
    setSize (340, 246);
}

//------------------------------------------------------------------------
WIDERAudioProcessorEditor::~WIDERAudioProcessorEditor()
{
    openGLComponentAttachment = nullptr;
}

//==============================================================================
void WIDERAudioProcessorEditor::paint (juce::Graphics& g)
{
//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

//------------------------------------------------------------------------
void WIDERAudioProcessorEditor::resized()
{

}
