/*
  ==============================================================================

    TextEditorAttachment.h
    Created: 14 May 2021 1:14:13pm
    Author:  migizo

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "AttachedControlBase.h"

class TextEditorAttachment : public AttachedControlBase, public TextEditor::Listener {
public:

    TextEditorAttachment(AudioProcessorValueTreeState& s,
                         const String& p, TextEditor& textEditor,
                         const String& prefix = "",
                         const String suffix = "",
                         bool populateGroup = true);
    ~TextEditorAttachment();
    void setValue(float newValue) override;

    void textEditorTextChanged(TextEditor& textEditor) override;
    void textEditorReturnKeyPressed (TextEditor& textEditor) override;
    void textEditorEscapeKeyPressed (TextEditor& textEditor) override;
    void textEditorFocusLost (TextEditor& textEditor) override;
    void onChangeTextEditor(TextEditor &textEditor);
private:
    float textToValue(const String& text);
    String valueToText(float value);
    String adjust(const String& text);
    TextEditor& textEditor;
    ScopedPointer<NormalisableRange<float>>range;
    float invInterval;

    bool ignoreCallbacks;
    CriticalSection selfCallbackMutex;
    
    String prefix;
    String suffix;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TextEditorAttachment)
};
