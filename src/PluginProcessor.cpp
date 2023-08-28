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
    , markovModel{}, elapsedSamples{ 0 }, chordDetect{ 100 }, learningMode{ true }, modelPlayNoteTime{ 0 }, lastNoteOnTime{ 0 }, noteDurationModel{}
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
    chordDetect = ChordDetector((unsigned long)maxIntervalInSamples);
    //std::cout << "Max interval in samples " << maxIntervalInSamples << " or " << (maxIntervalInSamples * 96000 * 1000) << "ms";

    /*juce::dsp::ProcessSpec spec;

    spec.maximumBlockSize = samplesPerBlock;

    spec.numChannels = 1;

    spec.sampleRate = sampleRate;

    leftChain.prepare(spec);
    rightChain.prepare(spec);

    auto chainSettings = getChainSettings(apvts);

    auto cutCoefficients = juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(chainSettings.lowCutFreq, 
                                                                                                        getSampleRate(),
                                                                                                        2*(chainSettings.lowCutSlope + 1));
    auto& leftLowCut = leftChain.get<ChainPositions::LowCut>();

    leftLowCut.setBypassed<0>(true);
    leftLowCut.setBypassed<1>(true);
    leftLowCut.setBypassed<2>(true);
    leftLowCut.setBypassed<3>(true);

    switch (ChainSettings.lowCutSlope)
    {
    case Slope_12:
    {
        *leftLowCut.get<0>().coefficients = cutCoefficients[0];
        leftLowCut.setBypassed<0>(false);
        break;
    }
        
    case Slope_24:
    {
        *leftLowCut.get<0>().coefficients = cutCoefficients[0];
        leftLowCut.setBypassed<0>(false);
        *leftLowCut.get<1>().coefficients = cutCoefficients[1];
        leftLowCut.setBypassed<1>(false);
        break;
    }
    case Slope_36:
    {
        *leftLowCut.get<0>().coefficients = cutCoefficients[0];
        leftLowCut.setBypassed<0>(false);
        *leftLowCut.get<1>().coefficients = cutCoefficients[1];
        leftLowCut.setBypassed<1>(false);
        *leftLowCut.get<2>().coefficients = cutCoefficients[2];
        leftLowCut.setBypassed<2>(false);
        break;
    }
    case Slope_48:
    {
        *leftLowCut.get<0>().coefficients = cutCoefficients[0];
        leftLowCut.setBypassed<0>(false);
        *leftLowCut.get<1>().coefficients = cutCoefficients[1];
        leftLowCut.setBypassed<1>(false);
        *leftLowCut.get<2>().coefficients = cutCoefficients[2];
        leftLowCut.setBypassed<2>(false);
        *leftLowCut.get<3>().coefficients = cutCoefficients[3];
        leftLowCut.setBypassed<3>(false);
        break;
    }

    }

    auto& rightLowCut = rightChain.get<ChainPositions::LowCut>();

    rightLowCut.setBypassed<0>(true);
    rightLowCut.setBypassed<1>(true);
    rightLowCut.setBypassed<2>(true);
    rightLowCut.setBypassed<3>(true);

    switch (chainSettings.lowCutSlope)
    {
    case Slope_12:
    {
        *rightLowCut.get<0>().coefficients = cutCoefficients[0];
        rightLowCut.setBypassed<0>(false);
        break;
    }

    case Slope_24:
    {
        *rightLowCut.get<0>().coefficients = cutCoefficients[0];
        rightLowCut.setBypassed<0>(false);
        *rightLowCut.get<1>().coefficients = cutCoefficients[1];
        rightLowCut.setBypassed<1>(false);
        break;
    }
    case Slope_36:
    {
        *rightLowCut.get<0>().coefficients = cutCoefficients[0];
        rightLowCut.setBypassed<0>(false);
        *rightLowCut.get<1>().coefficients = cutCoefficients[1];
        rightLowCut.setBypassed<1>(false);
        *rightLowCut.get<2>().coefficients = cutCoefficients[2];
        rightLowCut.setBypassed<2>(false);
        break;
    }
    case Slope_48:
    {
        *rightLowCut.get<0>().coefficients = cutCoefficients[0];
        rightLowCut.setBypassed<0>(false);
        *rightLowCut.get<1>().coefficients = cutCoefficients[1];
        rightLowCut.setBypassed<1>(false);
        *rightLowCut.get<2>().coefficients = cutCoefficients[2];
        rightLowCut.setBypassed<2>(false);
        *rightLowCut.get<3>().coefficients = cutCoefficients[3];
        rightLowCut.setBypassed<3>(false);
        break;
    }

    }*/


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
    //auto chainSettings = getChainSettings(apvts);

    DBG("ElapsedSamples" << elapsedSamples);
    addUIMessageToBuffer(midiMessages);


    juce::MidiBuffer generatedMessages{};

    /*if (isTimeToPlayNote()) {
        playNotesFromModel(generatedMessages);
        updateTimeForNextPlay();
    }*/

    //Newstuff

    /*juce::dsp::AudioBlock<float> block(buffer);

    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);*/

    //Finishes new stuff


    for (const auto metadata : midiMessages) {
        auto message = metadata.getMessage();
        if (message.isNoteOff()) {
            DBG("MIDI note off-learn duration");
            learnDuration(message);

            /*DBG("n:" << message.getNoteNumber()
                << "lasted "
                << );*/

        }


        if (message.isNoteOn()) {
            DBG("MIDI note on");
            noteOnTimes[message.getNoteNumber()] = elapsedSamples;
            chordDetect.addNote(message.getNoteNumber(), elapsedSamples + message.getTimeStamp());



            noteOffTimes[message.getNoteNumber()] = elapsedSamples + message.getTimeStamp();


            learnNotes();

            playNotesFromModel(generatedMessages);


        }

    }

    
    addNoteOffs(generatedMessages);

    midiMessages.clear();
    midiMessages.addEvents(generatedMessages, generatedMessages.getFirstEventTime(), -1, 0);
    elapsedSamples += buffer.getNumSamples();

    

    //SEPARACION



    //// temporary midi buffer 
    //juce::MidiBuffer generatedMessages{};
    //for (const auto metadata : midiMessages) {
    //    auto message = metadata.getMessage();
    //    if (message.isNoteOn()) {

    //        DBG("note "
    //            << message.getNoteNumber()
    //            << " ts: "
    //            << message.getTimeStamp());

    //        markovModel.putEvent(std::to_string(message.getNoteNumber()));
    //        int note = std::stoi(markovModel.getEvent());
    //        juce::MidiMessage nOn = juce::MidiMessage::noteOn(1, note, 0.5f);
    //        generatedMessages.addEvent(nOn, message.getTimeStamp());
    //        DBG("Markov model: " << markovModel.getModelAsString());

    //        juce::MidiMessage nOff = juce::MidiMessage::noteOff(1, note, 0.5f);
    //        generatedMessages.addEvent(nOff, message.getTimeStamp() + getSampleRate() / 2);
    //        
    //    }
    //}

    //if (midiToProcess.getNumEvents() > 0) {
    //    midiMessages.addEvents(midiToProcess, midiToProcess.getFirstEventTime(), midiToProcess.getLastEventTime() + 1, 0);
    //    midiToProcess.clear();
    //}

    //juce::MidiBuffer generatedMessages{};
    //for (const auto metadata : midiMessages) {
    //    auto message = metadata.getMessage();
    //    if (message.isNoteOn()) {
    //        chordDetect.addNote(
    //            message.getNoteNumber(),
    //            // add the offset within this buffer
    //            elapsedSamples + message.getTimeStamp()
    //        );
    //        if (chordDetect.hasChord()) {
    //            std::string notes =
    //                MidiMarkovProcessor::notesToMarkovState(
    //                    chordDetect.getChord()
    //                );
    //            markovModel.putEvent(notes);
    //        }

    //    }
    //}

    //if (chordDetect.hasChord()) {
    //    std::string notes = markovModel.getEvent();
    //    for (const int& note : markovStateToNotes(notes)) {
    //        juce::MidiMessage nOn = juce::MidiMessage::noteOn(1, note, 0.5f);
    //        generatedMessages.addEvent(nOn, 0);
    //        // remember to note off later
    //        noteOffTimes[note] = elapsedSamples + getSampleRate();
    //    }
    //}


    //// process pending note offs
    //for (auto i = 0; i < 127; ++i) {
    //    if (noteOffTimes[i] > 0 &&
    //        noteOffTimes[i] < elapsedSamples) {
    //        juce::MidiMessage nOff = juce::MidiMessage::noteOff(1, i, 0.0f);
    //        generatedMessages.addEvent(nOff, 0);
    //        noteOffTimes[i] = 0;
    //    }
    //}

    //midiMessages.clear();
    //midiMessages.addEvents(generatedMessages, generatedMessages.getFirstEventTime(), -1, 0);
    //elapsedSamples += buffer.getNumSamples();
    ////}

}

//==============================================================================
bool MidiMarkovProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* MidiMarkovProcessor::createEditor()
{   //
    return new MidiMarkovEditor(*this);
    //return new juce::GenericAudioProcessorEditor(*this);
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

//ChainSettings getChainSettings(juce::AudioProcessorValueTreeState& apvts)
//{
//    ChainSettings settings;
//
//    settings.lowCutFreq = apvts.getRawParameterValue("LowCutFilter")->load();
//    settings.highCutFreq = apvts.getRawParameterValue("HighCutFilter")->load();
//    settings.lowCutSlope = static_cast<Slope>(apvts.getRawParameterValue("LowCut Slope")->load());
//    settings.highCutSlope = static_cast<Slope>(apvts.getRawParameterValue("HighCut Slope")->load());
//
//    return settings;
//}

//juce::AudioProcessorValueTreeState::ParameterLayout MidiMarkovProcessor::createParameterLayout() 
//{
//    //We'll have a low pas filter and a high pass filter
//    //LowCut parameters
//    juce::AudioProcessorValueTreeState::ParameterLayout layout;
//    layout.add(std::make_unique<juce::AudioParameterFloat>("LowCutFilter", 
//                                                           "LowCutFilter", 
//                                                            juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),
//                                                            20.f));
//
//    //HighCut parametes
//    layout.add(std::make_unique<juce::AudioParameterFloat>("HighCutFilter",
//                                                           "HighCutFilter",
//                                                            juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 0.25f),
//                                                            20000.f));
//
//    juce::StringArray stringArray;
//    for (int i = 0; i < 4; ++i) {
//        juce::String str;
//        str << (12 + i * 12);
//        str << " db/Oct";
//        stringArray.add(str);
//    }
//
//    layout.add(std::make_unique<juce::AudioParameterChoice>("LowCut Slope", "LowCut Slope", stringArray, 0));
//    layout.add(std::make_unique<juce::AudioParameterChoice>("HighCut Slope", "HighCut Slope", stringArray, 0));
//
//
//    return layout;
//
//}

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

void MidiMarkovProcessor::resetModel()
{
    markovModel.reset();
    noteDurationModel.reset();
    for (auto i = 0; i < 127; ++i) {
        noteOffTimes[i] = 0;
        noteOnTimes[i] = 0;
    }
}

bool MidiMarkovProcessor::isLearning()
{
    return learningMode;
}
void MidiMarkovProcessor::stopLearning()
{
    learningMode = false;
}
void MidiMarkovProcessor::startLearning()
{
    learningMode = true;
}

bool MidiMarkovProcessor::isTimeToPlayNote()
{
    if (modelPlayNoteTime == 0) {
        modelPlayNoteTime = elapsedSamples;
        return false;
    }
    else { return elapsedSamples > modelPlayNoteTime; }
}


//// modular functions for processblock

void MidiMarkovProcessor::addUIMessageToBuffer(juce::MidiBuffer& midiMessages)
{
    if (midiToProcess.getNumEvents() > 0) {
        midiMessages.addEvents(midiToProcess, midiToProcess.getFirstEventTime(), midiToProcess.getLastEventTime() + 1, 0);
        midiToProcess.clear();
    }
}

void MidiMarkovProcessor::learnNotes() {
    if (learningMode) {
        if (chordDetect.hasChord()) {
            std::string notes = MidiMarkovProcessor::notesToMarkovState(chordDetect.getChord());
            markovModel.putEvent(notes);
        }
    }
}

void MidiMarkovProcessor::learnDuration(juce::MidiMessage& noteOffMessage)
{
    if (learningMode && noteOffMessage.isNoteOff()) {
        unsigned long noteLength =
            elapsedSamples - noteOnTimes[noteOffMessage.getNoteNumber()];

        noteDurationModel.putEvent(std::to_string(noteLength));
    }
}

void MidiMarkovProcessor::playNotesFromModel(juce::MidiBuffer& midiMessages)
{
    if (chordDetect.hasChord()) {
        std::string notes = markovModel.getEvent(true);

        unsigned int duration = std::stoi(noteDurationModel.getEvent(true));

        for (const int& note : markovStateToNotes(notes)) {
            DBG("MIDI note on se manda");
            juce::MidiMessage nOn = juce::MidiMessage::noteOn(1, note, 0.8f);
            midiMessages.addEvent(nOn, 0);
            
            // remember to note off later
            noteOffTimes[note] = elapsedSamples + duration;
        }

    }
}

void MidiMarkovProcessor::addNoteOffs(juce::MidiBuffer& midiMessages)
{
    // process pending note offs
    for (auto i = 0; i < 127; ++i) {
        if (noteOffTimes[i] > 0 &&
            noteOffTimes[i] < elapsedSamples) {
            juce::MidiMessage nOff = juce::MidiMessage::noteOff(1, i, 0.0f);
            midiMessages.addEvent(nOff, 0);
            noteOffTimes[i] = 0;
        }
    }
}


void MidiMarkovProcessor::updateTimeForNextPlay()
{
    // select the next time to play a note
    //modelPlayNoteTime = std::stoul(iOIModel.getEvent(true));
    modelPlayNoteTime += elapsedSamples;
}
