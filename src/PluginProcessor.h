#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "MarkovManager.h"


class MidiMarkovProcessor : public juce::AudioProcessor
    #if JucePlugin_Enable_ARA
    , public juce::AudioProcessorARAExtension
    #endif

{
public:
    //==============================================================================
    MidiMarkovProcessor();
    ~MidiMarkovProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    /** add some midi to be played at the sent sample offset*/
    void addMidi(juce::MidiMessage msg, int sampleOffset);

    ///**
    // * @brief reset the markov model
    // *
    // */
    //void resetModel();
    //// check if the markov model is learning from incoming midi
    //bool isLearning();
    //// stop the markov model from learning from incoming MIDI
    //void stopLearning();
    //// start the markov model learning from incoming MIDI
    //void startLearning();

private:

    MarkovManager markovModel;
    //MarkovManager noteDurationModel;
    //bool learningMode;

    /** stores messages added from the addMidi function*/
    juce::MidiBuffer midiToProcess;
    unsigned long noteOffTimes[127];
    unsigned long noteOnTimes[127];
    //unsigned long lastNoteOnTime;
    //unsigned long modelPlayNoteTime;
    unsigned long elapsedSamples;

    //// a set of functions to modularise the processBlock function
    //void addUIMessageToBuffer(juce::MidiBuffer& midiMessages);
    //// query the pitch and duration models and put somo midi notes into the sent buffer
    //void playNotesFromModel(juce::MidiBuffer& noteBuffer);
    //// adds note off messages to the sent buffer, if needed based on noteOffTimes and elapsedSamples
    //void addNoteOffs(juce::MidiBuffer& midiMessages);
    //// add the notes stored in the chord detector to the model 
    ////void learnNotes();
    //// compute the duration of the sent message (based on noteOnTimes)
    //// and add it to the duration model
    //void learnDuration(juce::MidiMessage& noteOffMessage);
    //// returns true if elapsedTime > modelPlayNoteTime 
    //bool isTimeToPlayNote();
    //// select next modelPlayNoteTime by querying the IOI model 
    //void updateTimeForNextPlay();
    //// end of processBlock modular functions

    /**
     * @brief converts a vector of midi note values to a single string representation
     *
     * @param notesVec
     * @return std::string
     */
    static std::string notesToMarkovState(const std::vector<int>& notesVec);
    /**
     * @brief converts a string representation of notes from notesToMarkovState back to an int vector of midi note values
     *
     * @param notesStr
     * @return std::vector<int>
     */
    static std::vector<int> markovStateToNotes(const std::string& notesStr);



    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiMarkovProcessor)
};
