/*
  ==============================================================================

    OpenGLComponent.cpp
    Created: 11 May 2021 12:11:05am
    Author:  migizo
    reference: https://medium.com/@Im_Jimmi/using-opengl-for-2d-graphics-in-a-juce-plug-in-24aa82f634ff
  ==============================================================================
*/

#include <JuceHeader.h>
#include "OpenGLComponent.h"

//==============================================================================
OpenGLComponent::OpenGLComponent()
{
    
    // true = 不透明であると明示, false = transparent
    setOpaque (false);
    
    // レンダラーとしてセット
    openGLContext.setRenderer(this);

    // repaint loop ON
    openGLContext.setContinuousRepainting(true);

    // コンポーネントをopenGLにアタッチ
    openGLContext.attachTo(*this);
    
    // pluginパッケージのResourcesフォルダのshaderを読む
    String resourcePath = File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getFullPathName();
#ifdef JUCE_MAC
    resourcePath += "/Contents/Resources/";
#elif JUCE_WINDOWS
#endif
    vertexShaderFile = File(resourcePath + "shader.vert");
    fragmentShaderFile = File(resourcePath + "shader.frag");
    vertexShader = vertexShaderFile.loadFileAsString();
    fragmentShader = fragmentShaderFile.loadFileAsString();
    vertexShaderFile.setLastAccessTime(Time::getCurrentTime());
    fragmentShaderFile.setLastAccessTime(Time::getCurrentTime());
    vertexLastModTime = vertexShaderFile.getLastModificationTime();
    fragmentLastModTime = fragmentShaderFile.getLastModificationTime();
    
    startTimerHz(60);
}

//------------------------------------------------------------------------
OpenGLComponent::~OpenGLComponent()
{
    stopTimer();
    openGLContext.detach();
}

//------------------------------------------------------------------------
void OpenGLComponent::newOpenGLContextCreated() {
    
    // バッファに送信するデータの作成
    vertexBuffer =
    {
        // Vertex 0
        {
            { -1.0f, 1.0f },        //
            { 1.f, 0.f, 0.f, 1.f }  // Red
        },
        // Vertex 1
        {
            { 1.0f, 1.0f },         //
            { 1.f, 0.5f, 0.f, 1.f } // Orange
        },
        // Vertex 2
        {
            { 1.0f, -1.0f },        //
            { 1.f, 1.f, 0.f, 1.f }  // Yellow
        },
        // Vertex 3
        {
            { -1.0f, -1.0f },       //
            { 1.f, 0.f, 1.f, 1.f }  // Purple
        }
    };
    
    // We need 6 indices, 1 for each corner of the two triangles.
    indexBuffer = {
        0, 1, 2,
        0, 2, 3
    };
    
    // バッファ生成
    openGLContext.extensions.glGenBuffers(1, &vbo);
    openGLContext.extensions.glGenBuffers(1, &ibo);
    
    /*
     openGLContext.extensions.glBufferData([0], [1], [2], [3])
     [0]: 送信するデータ型.
     [1]: データのbyteサイズ
     [2]: データのポインタ.
     [3]: どのように描画されるか.
    */
    {
        // バッファをバインド(vertex)
        openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vbo);
        
        // データを送信
        auto databyteSize = sizeof(Vertex) * vertexBuffer.size();
        openGLContext.extensions.glBufferData(GL_ARRAY_BUFFER, databyteSize, vertexBuffer.data(), GL_STATIC_DRAW);
    }
    {
        // バッファをバインド(index)
        openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        
        // データを送信
        auto databyteSize = sizeof(unsigned int) * indexBuffer.size();
        openGLContext.extensions.glBufferData(GL_ELEMENT_ARRAY_BUFFER, databyteSize, indexBuffer.data(), GL_STATIC_DRAW);
    }

    shaderCompileAndLink();
}

//------------------------------------------------------------------------
void OpenGLComponent::shaderCompileAndLink() {
    
    std::unique_ptr<OpenGLShaderProgram> newShader (new OpenGLShaderProgram (openGLContext));

    // コンパイルとリンク
    bool compileVertex = newShader->addVertexShader(vertexShader);
    bool compileFragment = newShader->addFragmentShader(fragmentShader);
    if (compileVertex && compileFragment && newShader->link())
    {
        shaderProgram = std::move (newShader);
        
        // shaderProgramをアクティブ化,プログラムを使用するよう指示
        shaderProgram->use();
        
        std::cout << "GLSL: v" + String (OpenGLShaderProgram::getLanguageVersion(), 2) << std::endl;
    }
    else
    {
        std::cout << "failed: " << shaderProgram->getLastError() << std::endl;

        // エラーチェック
        jassertfalse;
    }
}

void OpenGLComponent::renderOpenGL() {
    auto currentTime = Time::getCurrentTime();
    bool isStandbyUpdateShader = false;
    if (currentTime.toMilliseconds() - vertexShaderFile.getLastAccessTime().toMilliseconds() > 1000) {
        vertexShaderFile.setLastAccessTime(currentTime);
        if (vertexLastModTime != vertexShaderFile.getLastModificationTime()) {
            vertexShaderFile.setLastModificationTime(vertexLastModTime);
            vertexShader = vertexShaderFile.loadFileAsString();
            isStandbyUpdateShader = true;
            std::cout << "change vertex" << std::endl;
        }
    }
    if (currentTime.toMilliseconds() - fragmentShaderFile.getLastAccessTime().toMilliseconds() > 1000) {
        fragmentShaderFile.setLastAccessTime(currentTime);
        if (fragmentLastModTime != fragmentShaderFile.getLastModificationTime()) {
            fragmentShaderFile.setLastModificationTime(fragmentLastModTime);
            fragmentShader = fragmentShaderFile.loadFileAsString();
            isStandbyUpdateShader = true;
            std::cout << "change fragment" << std::endl;
        }
    }
    if (isStandbyUpdateShader) {
        shaderCompileAndLink();
    }
    
    // clear
    OpenGLHelpers::clear(Colour::fromRGBA(70, 160, 170, 0));
    
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // フレーム毎にプログラムを使用するように指示
    shaderProgram->use();
    
    // shaderがリンクされていればshaderコード上の変数を取得でき、値を設定できる
    shaderProgram->setUniform("time", timeCounter);
    shaderProgram->setUniform("value", value/100.0f);
    shaderProgram->setUniform("isHover", isHover ? 1 : 0);

    // VBOとIBOをバインド
    openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vbo);
    openGLContext.extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    
    // 使用する位置attributeを有効にする
    openGLContext.extensions.glVertexAttribPointer(
        0,              // attributeのindex
        2,              // いくつの値を保持しているattributeか
        GL_FLOAT,       // attributeの型
        GL_FALSE,       // 値を正規化するかどうか
        sizeof(Vertex), // 次の位置attributeまでのbytesize(xyrgba,xy...)
        nullptr         // attributeの開始byte
    );
    openGLContext.extensions.glEnableVertexAttribArray(0);
        
    // 使用する色attributeを有効にする
    openGLContext.extensions.glVertexAttribPointer(
        1,                              // attributeのindex
        4,                              // いくつの値を保持しているattributeか
        GL_FLOAT,                       // attributeの型
        GL_FALSE,                       // 値を正規化するかどうか
        sizeof(Vertex),                 // 次の位置attributeまでのbytesize(xy,rgbaxy,rgba...)
        (GLvoid*)(sizeof(float) * 2)    // attributeの開始byte
    );
    openGLContext.extensions.glEnableVertexAttribArray(1);
    
    glDrawElements(
            GL_TRIANGLES,       // meshの描画モード
            (GLsizei)indexBuffer.size(), // インデックスのサイズ
            GL_UNSIGNED_INT,    // インデックスの型
            nullptr             // 既にインデックスをopenGLに渡しているのでここではパスする
        );
    
    openGLContext.extensions.glDisableVertexAttribArray(0);
    openGLContext.extensions.glDisableVertexAttribArray(1);
    
    timeCounter += 0.02f;
}

//------------------------------------------------------------------------
void OpenGLComponent::paint (juce::Graphics& g)
{
    // openGLの上に描画される
}

//------------------------------------------------------------------------
void OpenGLComponent::resized()
{
}

//------------------------------------------------------------------------
void OpenGLComponent::mouseDown(const juce::MouseEvent& event) {
    float mouseX = jmap((double)event.x, 20.0, (double)getWidth() - 20.0, 0.0, 1.0);
    mouseX = std::min (std::max (0.0f, mouseX), 1.0f);
    float newValue = abs(mouseX - 0.5) * 2.0;
    newValue *= 100.0;
    
    // 実際に値が変更されるtimerCallback内で値変更通知
    animator.start(10, value, newValue);
}

//------------------------------------------------------------------------
void OpenGLComponent::mouseDrag (const MouseEvent& event)
{
    float mouseX = jmap((double)event.x, 20.0, (double)getWidth() - 20.0, 0.0, 1.0);
    mouseX = std::min (std::max (0.0f, mouseX), 1.0f);
    float newValue = abs(mouseX - 0.5) * 2.0;
    newValue *= 100.0;
    
    // クリック時のアニメーションが走ってなければ値更新
    if (! animator.getIsProgress()) {
        value = newValue;
        listeners.call(&Listener::valueChanged, this, value);
    }
}

//------------------------------------------------------------------------
void OpenGLComponent::mouseEnter(const juce::MouseEvent& event) {
    isHover = true;
}

//------------------------------------------------------------------------
void OpenGLComponent::mouseExit(const juce::MouseEvent& event) {
    isHover = false;
}

//------------------------------------------------------------------------
void OpenGLComponent::setValue(float newValue, const NotificationType notification) {
    value = newValue;
    if (notification != dontSendNotification)
        listeners.call(&Listener::valueChanged, this, value);
}

//------------------------------------------------------------------------
void OpenGLComponent::setValue() {
    value = defaultValue;
}

//------------------------------------------------------------------------
void OpenGLComponent::timerCallback()
{
    animator.update();
    if (animator.getIsProgress()) {
        value = animator.getValue();
        listeners.call(&Listener::valueChanged, this, value);
    }
}
//==============================================================================
// attachment
//==============================================================================
OpenGLComponentAttachment::OpenGLComponentAttachment(AudioProcessorValueTreeState& s, const String& p, OpenGLComponent& comp, bool populateGroup)
    : AttachedControlBase(s, p), comp(comp),  ignoreCallbacks(false)
{
    range = new NormalisableRange<float>(s.getParameterRange(p));
    if (range->interval == 0.f) return;
    invInterval = 1.f / range->interval;
        
    // 正規化した値が来るので0-100に直す
    float defaultValue = range->start + (range->end - range->start) * getParameter()->get();
    comp.setDefaultValue(defaultValue);
    
    comp.resized();
    sendInitialUpdate();
    comp.addListener(this);
}

//------------------------------------------------------------------------
OpenGLComponentAttachment::~OpenGLComponentAttachment() {
    AttachedControlBase::removeListener();
    comp.removeListener(this);
}

//------------------------------------------------------------------------
void OpenGLComponentAttachment::setValue(float newValue) {
    const ScopedLock selfCallbackLock(selfCallbackMutex);
    {
        ScopedValueSetter<bool> svs(ignoreCallbacks, true);
        comp.setValue(newValue, sendNotificationSync);
    }
}

//------------------------------------------------------------------------
void OpenGLComponentAttachment::valueChanged(OpenGLComponent* comp, float newValue) {
    const ScopedLock selfCallbackLock(selfCallbackMutex);
    if ((!ignoreCallbacks) && (!ModifierKeys::getCurrentModifiers().isRightButtonDown()))
        setNewUnnormalisedValue(newValue);
}
