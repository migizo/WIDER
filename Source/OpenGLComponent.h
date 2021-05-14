/*
  ==============================================================================

    OpenGLComponent.h
    Created: 11 May 2021 12:11:05am
    Author:  migizo

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "AttachedControlBase.h"
#include "AnimatorByCount.h"
//==============================================================================
/*
*/
class OpenGLComponent  : public juce::Component,
                         public OpenGLRenderer,
                         private juce::Timer
{
public:
    class Listener {
    public:
        virtual void valueChanged(OpenGLComponent* comp, float newValue) = 0;
    };
    
    OpenGLComponent();
    ~OpenGLComponent() override;

    // openGLRenderer
    void newOpenGLContextCreated() override;        // コンテキストが作成された時に呼ばれる
    void renderOpenGL() override;
    void openGLContextClosing()override {};

    // component
    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag (const MouseEvent& event) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;

    // for attachment
    void setDefaultValue(float newDefaultValue) {defaultValue = newDefaultValue; }
    void setValue(float newValue, const NotificationType notification);
    void setValue();
    float getValue() { return value;}
    void addListener(Listener* listener) {listeners.add(listener); }
    void removeListener(Listener* listener) {listeners.remove(listener); }

private:
    void timerCallback() override;

    // shader,openGL
    void shaderCompileAndLink();
    OpenGLContext openGLContext;
    
    struct Vertex
    {
        float position[2];
        float colour[4];
    };
    std::vector<Vertex> vertexBuffer;
    std::vector<unsigned int> indexBuffer;
    
    GLuint vbo; // Vertex buffer object.
    GLuint ibo; // Index buffer object.
    
    String vertexShader;
    String fragmentShader;
    
    File vertexShaderFile;
    File fragmentShaderFile;
    Time vertexLastModTime;
    Time fragmentLastModTime;

    // shadeerのコンパイルとリンク
    std::unique_ptr<OpenGLShaderProgram> shaderProgram;
    
    // uniform
    GLfloat timeCounter = 0.0f;
    bool isHover = false;
    
    AnimatorByCount animator;
    
    float value, defaultValue;
    ListenerList<Listener> listeners;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLComponent)
};


//==============================================================================
class OpenGLComponentAttachment : public AttachedControlBase, public OpenGLComponent::Listener {
public:

    OpenGLComponentAttachment(AudioProcessorValueTreeState& s, const String& p, OpenGLComponent& comp, bool populateGroup = true);
    ~OpenGLComponentAttachment();
    void setValue(float newValue) override;

    void valueChanged(OpenGLComponent* comp, float newValue) override;
    
private:
    OpenGLComponent& comp;
    ScopedPointer<NormalisableRange<float>>range;
    float invInterval;

    bool ignoreCallbacks;
    CriticalSection selfCallbackMutex;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OpenGLComponentAttachment)
};
