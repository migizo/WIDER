/*
  ==============================================================================

    TextEditorAttachment.cpp
    Created: 14 May 2021 1:18:55pm
    Author:  migizo

  ==============================================================================
*/

#include "TextEditorAttachment.h"
TextEditorAttachment::TextEditorAttachment(AudioProcessorValueTreeState& s,
                     const String& p, TextEditor& textEditor,
                     const String& prefix, const String suffix, bool populateGroup)
                    : AttachedControlBase(s, p), textEditor(textEditor), prefix(prefix), suffix(suffix), ignoreCallbacks(false)
{
    range = new NormalisableRange<float>(s.getParameterRange(p));
    if (range->interval == 0.f) return;
    invInterval = 1.f / range->interval;
        
    // 正規化した値が来るので0-100に直す
    float defaultValue = range->start + (range->end - range->start) * getParameter()->get();
    textEditor.setText(valueToText(defaultValue), false);
    
    textEditor.resized();
    sendInitialUpdate();
    textEditor.addListener(this);
}

//------------------------------------------------------------------------
TextEditorAttachment::~TextEditorAttachment()
{
    AttachedControlBase::removeListener();
    textEditor.removeListener(this);
}

//------------------------------------------------------------------------
void TextEditorAttachment::setValue(float newValue)
{
    const ScopedLock selfCallbackLock(selfCallbackMutex);
    {
        ScopedValueSetter<bool> svs(ignoreCallbacks, true);
        textEditor.setText(valueToText(newValue), true);
    }
}

//------------------------------------------------------------------------
void TextEditorAttachment::textEditorTextChanged(TextEditor& textEditor)
{
    onChangeTextEditor(textEditor);
}

//------------------------------------------------------------------------
void TextEditorAttachment::textEditorReturnKeyPressed (TextEditor& textEditor)
{
    onChangeTextEditor(textEditor);
}

//------------------------------------------------------------------------
void TextEditorAttachment::textEditorEscapeKeyPressed (TextEditor& textEditor)
{
    onChangeTextEditor(textEditor);
}

//------------------------------------------------------------------------
void TextEditorAttachment::textEditorFocusLost (TextEditor& textEditor)
{
    onChangeTextEditor(textEditor);
}

//------------------------------------------------------------------------
void TextEditorAttachment::onChangeTextEditor(TextEditor &textEditor) {
    const ScopedLock selfCallbackLock(selfCallbackMutex);
    if ((!ignoreCallbacks) && (!ModifierKeys::getCurrentModifiers().isRightButtonDown())) {
        
        setNewUnnormalisedValue(textToValue(textEditor.getText()));
        textEditor.setText(adjust(textEditor.getText()));
    }
}

//------------------------------------------------------------------------
float TextEditorAttachment::textToValue(const String& text) {
    String newText = text;
    if (prefix != "") {
        if (newText.startsWith(prefix)) {
            newText = newText.substring(prefix.length());
        }
    }
    return newText.trim().getFloatValue();
}

//------------------------------------------------------------------------
String TextEditorAttachment::valueToText(float value){
    return prefix + String(value) + suffix;
}

//------------------------------------------------------------------------
String TextEditorAttachment::adjust(const String& text){
    return valueToText(textToValue(text));
}
