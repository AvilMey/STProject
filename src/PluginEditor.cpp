

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MidiMarkovEditor::MidiMarkovEditor(MidiMarkovProcessor& p) : AudioProcessorEditor(&p), audioProcessor(p),
    miniPianoKbd{ kbdState, juce::MidiKeyboardComponent::horizontalKeyboard }

{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(700, 350);

    // listen to the mini piano
    kbdState.addListener(this);
    addAndMakeVisible(miniPianoKbd);

    resetButton.setButtonText("Reset model");
    addAndMakeVisible(resetButton);
    resetButton.addListener(this);

    isLearningButton.setButtonText("Machine is learning");
    addAndMakeVisible(isLearningButton);
    isLearningButton.addListener(this);
}

MidiMarkovEditor::~MidiMarkovEditor()
{
}

//==============================================================================
void MidiMarkovEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    g.drawFittedText("v0.001", getLocalBounds(), juce::Justification::centred, 1);
}

void MidiMarkovEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    float rowHeight = getHeight() / 5;
    float colWidth = getWidth() / 3;
    float row = 0;
    float col = 0;


    miniPianoKbd.setBounds(0, rowHeight * row, getWidth(), rowHeight);
    row++;
    resetButton.setBounds(0, rowHeight * row, colWidth, rowHeight);
    col++;
    isLearningButton.setBounds(col * colWidth, rowHeight * row, colWidth, rowHeight);

}

void MidiMarkovEditor::sliderValueChanged(juce::Slider* slider)
{

}

void MidiMarkovEditor::buttonClicked(juce::Button* btn)
{
    if (btn == &resetButton) {
        this->audioProcessor.resetModel();
    }
    if (btn == &isLearningButton) {
        // if processor is learning, set to 'is not learning'
        if (this->audioProcessor.isLearning()) {
            this->audioProcessor.stopLearning();
            // make the button off
            isLearningButton.setButtonText("Not learning");
        }
        else {
            this->audioProcessor.startLearning();
            isLearningButton.setButtonText("Machine is learning");
        }
    }
}

void MidiMarkovEditor::handleNoteOn(juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity)
{
    juce::MidiMessage msg1 = juce::MidiMessage::noteOn(midiChannel, midiNoteNumber, velocity);
    audioProcessor.addMidi(msg1, 0);

}

void MidiMarkovEditor::handleNoteOff(juce::MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity)
{
    juce::MidiMessage msg2 = juce::MidiMessage::noteOff(midiChannel, midiNoteNumber, velocity);
    audioProcessor.addMidi(msg2, 0);
}
