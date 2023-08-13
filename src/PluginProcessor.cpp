#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MidiMarkovProcessor::MidiMarkovProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
    , markovModel{},  elapsedSamples{ 0 } //learningMode{ true } noteDurationModel {}, lastNoteOnTime{ 0 }, modelPlayNoteTime{ 0 },
{
    for (auto i = 0; i < 127; ++i) {
        noteOffTimes[i] = 0;
        noteOnTimes[i] = 0;
    }
}


MidiMarkovProcessor::~MidiMarkovProcessor()
{
}

//==============================================================================
const juce::String MidiMarkovProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MidiMarkovProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool MidiMarkovProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool MidiMarkovProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double MidiMarkovProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MidiMarkovProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int MidiMarkovProcessor::getCurrentProgram()
{
    return 0;
}

void MidiMarkovProcessor::setCurrentProgram(int index)
{
}

const juce::String MidiMarkovProcessor::getProgramName(int index)
{
    return {};
}

void MidiMarkovProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void MidiMarkovProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // here we know the sample rate so can set the
    // maxinterval properly on the chord detector
    // 0.5ms seems to be about right
    double maxIntervalInSamples = sampleRate * 0.05; // 50ms
    //std::cout << "Max interval in samples " << maxIntervalInSamples << " or " << (maxIntervalInSamples * 96000 * 1000) << "ms";
}

void MidiMarkovProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MidiMarkovProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void MidiMarkovProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    

    if (midiToProcess.getNumEvents() > 0) {
        midiMessages.addEvents(midiToProcess, midiToProcess.getFirstEventTime(), midiToProcess.getLastEventTime() + 1, 0);
        midiToProcess.clear();
    }

    /*for (const auto metadata : midiMessages) {
        auto message = metadata.getMessage();
        if (message.isNoteOn()) {
            std::cout << "note "
                << message.getNoteNumber()
                << " ts: "
                << message.getTimeStamp()
                << std::endl;
        }
    }*/
    // temporary midi buffer 
    juce::MidiBuffer generatedMessages{};
    for (const auto metadata : midiMessages) {
        auto message = metadata.getMessage();
        if (message.isNoteOn()) {

            markovModel.putEvent(std::to_string(message.getNoteNumber()));
            int note = std::stoi(markovModel.getEvent());
            juce::MidiMessage nOn = juce::MidiMessage::noteOn(1, note, 0.5f);
            generatedMessages.addEvent(nOn, message.getTimeStamp());
            DBG("Markov model: " << markovModel.getModelAsString());

            juce::MidiMessage nOff = juce::MidiMessage::noteOff(1, note, 0.5f);
            generatedMessages.addEvent(nOff, message.getTimeStamp() + getSampleRate() / 2);
            
            //chordDetect.addNote(
            //    message.getNoteNumber(),
            //    // add the offset within this buffer
            //    elapsedSamples + message.getTimeStamp()
            //);
            //if (chordDetect.hasChord()) {
            //    std::string notes =
            //        MidiMarkovProcessor::notesToMarkovState(
            //            chordDetect.getChord()
            //        );
            //    markovModel.putEvent(notes);
            //}
        }
    }
    //if (chordDetect.hasChord()) {
    //    std::string notes = markovModel.getEvent();
    //    for (const int& note : markovStateToNotes(notes)) {
    //        juce::MidiMessage nOn = juce::MidiMessage::noteOn(1, note, 0.5f);
    //        generatedMessages.addEvent(nOn, 0);
    //        // remember to note off later
    //        noteOffTimes[note] = elapsedSamples + getSampleRate();
    //    }
    //}

    // process pending note offs
   /* for (auto i = 0; i < 127; ++i) {
        if (noteOffTimes[i] > 0 &&
            noteOffTimes[i] < elapsedSamples) {
            juce::MidiMessage nOff = juce::MidiMessage::noteOff(1, i, 0.0f);
            generatedMessages.addEvent(nOff, 0);
            noteOffTimes[i] = 0;
        }
    }*/
    midiMessages.clear();
    midiMessages.addEvents(generatedMessages, generatedMessages.getFirstEventTime(), -1, 0);
    elapsedSamples += buffer.getNumSamples();
//}



    //addUIMessageToBuffer(midiMessages);

    //// temporary midi buffer 
    //juce::MidiBuffer generatedMessages{};

    //if (isTimeToPlayNote()) {
    //    playNotesFromModel(generatedMessages);
    //    updateTimeForNextPlay();
    //}


    //for (const auto metadata : midiMessages) {
    //    auto message = metadata.getMessage();
    //    if (message.isNoteOff()) {
    //        learnDuration(message);
    //    }
    //    if (message.isNoteOn()) {
    //        std::cout << "note "
    //            << message.getNoteNumber()
    //            << " ts: "
    //            << message.getTimeStamp()
    //            << std::endl;

    //        // remember note on time for duration model 
    //        noteOnTimes[message.getNoteNumber()] = elapsedSamples + message.getTimeStamp();

    //    

    //    }
    //}

    //addNoteOffs(generatedMessages);

    //midiMessages.clear();
    //midiMessages.addEvents(generatedMessages, generatedMessages.getFirstEventTime(), -1, 0);
    //elapsedSamples += buffer.getNumSamples();
    //// initialise the modelPlayNoteTime if necessary
    ////if (modelPlayNoteTime == 0) {modelPlayNoteTime = elapsedSamples + getSampleRate();}
}

//==============================================================================
bool MidiMarkovProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MidiMarkovProcessor::createEditor()
{
    return new MidiMarkovEditor(*this);
}

//==============================================================================
void MidiMarkovProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MidiMarkovProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MidiMarkovProcessor();
}


void MidiMarkovProcessor::addMidi(juce::MidiMessage msg, int sampleOffset)
{
    midiToProcess.addEvent(msg, sampleOffset);
}


std::string MidiMarkovProcessor::notesToMarkovState(const std::vector<int>& notesVec)
{
    std::string state{ "" };
    for (const int& note : notesVec) {
        state += std::to_string(note) + "-";
    }
    return state;
}

std::vector<int> MidiMarkovProcessor::markovStateToNotes(const std::string& notesStr)
{
    std::vector<int> notes{};
    if (notesStr == "0") return notes;
    for (const std::string& note : MarkovChain::tokenise(notesStr, '-')) {
        notes.push_back(std::stoi(note));
    }
    return notes;
}

//void MidiMarkovProcessor::resetModel()
//{
//    markovModel.reset();
//    noteDurationModel.reset();
//    for (auto i = 0; i < 127; ++i) {
//        noteOffTimes[i] = 0;
//        noteOnTimes[i] = 0;
//    }
//}

//bool MidiMarkovProcessor::isLearning()
//{
//    return learningMode;
//}
//void MidiMarkovProcessor::stopLearning()
//{
//    learningMode = false;
//}
//void MidiMarkovProcessor::startLearning()
//{
//    learningMode = true;
//}

//bool MidiMarkovProcessor::isTimeToPlayNote()
//{
//    if (modelPlayNoteTime == 0) {
//        modelPlayNoteTime = elapsedSamples;
//        return false;
//    }
//    else { return elapsedSamples > modelPlayNoteTime; }
//}


//// modular functions for processblock

//void MidiMarkovProcessor::addUIMessageToBuffer(juce::MidiBuffer& midiMessages)
//{
//    if (midiToProcess.getNumEvents() > 0) {
//        midiMessages.addEvents(midiToProcess, midiToProcess.getFirstEventTime(), midiToProcess.getLastEventTime() + 1, 0);
//        midiToProcess.clear();
//    }
//}
//
//void MidiMarkovProcessor::learnDuration(juce::MidiMessage& noteOffMessage)
//{
//    if (learningMode && noteOffMessage.isNoteOff()) {
//        unsigned long noteLength =
//            elapsedSamples - noteOnTimes[noteOffMessage.getNoteNumber()];
//
//        noteDurationModel.putEvent(std::to_string(noteLength));
//    }
//}
//
//void MidiMarkovProcessor::playNotesFromModel(juce::MidiBuffer& midiMessages)
//{
//        std::string notes = markovModel.getEvent(true);
//
//        unsigned int duration = std::stoi(noteDurationModel.getEvent(true));
//
//        for (const int& note : markovStateToNotes(notes)) {
//            juce::MidiMessage nOn = juce::MidiMessage::noteOn(1, note, 0.8f);
//            midiMessages.addEvent(nOn, 0);
//            // remember to note off later
//            noteOffTimes[note] = elapsedSamples + duration;
//        }
//
//}
//
//void MidiMarkovProcessor::addNoteOffs(juce::MidiBuffer& midiMessages)
//{
//    // process pending note offs
//    for (auto i = 0; i < 127; ++i) {
//        if (noteOffTimes[i] > 0 &&
//            noteOffTimes[i] < elapsedSamples) {
//            juce::MidiMessage nOff = juce::MidiMessage::noteOff(1, i, 0.0f);
//            midiMessages.addEvent(nOff, 0);
//            noteOffTimes[i] = 0;
//        }
//    }
//}
//
//
//void MidiMarkovProcessor::updateTimeForNextPlay()
//{
//    // select the next time to play a note
//    //modelPlayNoteTime = std::stoul(iOIModel.getEvent(true));
//    modelPlayNoteTime += elapsedSamples;
//}



