/*
  ==============================================================================

    WaveTable.cpp
    Created: 26 Dec 2025 9:39:19pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#include "WaveTable.h"

//Initializer function, called during processor construction
void WaveBankManager::loadWavetablesFromBinary()
{
    int totalFilesLoaded = 0;
    //Loop through assetID and load binary files
    for (int i = 0; i < (int)WaveBankManager::AssetID::Count; ++i)
    {
        auto id = static_cast<WaveBankManager::AssetID>(i);

        size_t size;
        const char* data = WaveBankManager::getAssetData(id, size);
        
        
        auto memStream = std::make_unique<juce::MemoryInputStream>(
                        data,
                        size,
                        false );
        
        
        std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(std::move(memStream)));
        
        if (reader != nullptr)
        {
            // Create an AudioBuffer with the right number of channels and samples
            juce::AudioBuffer<float> buffer(reader->numChannels, static_cast<int>(reader->lengthInSamples));

            // Read all samples into the buffer
            reader->read(&buffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
            
            //Parse file, need to take buffer, split it up into wavetables and store them in the wavebank
            Wavetable newTable = parseBinaryData(buffer);
            newTable.name = "Test_" + juce::String(i);
            waveTables.push_back(std::make_shared<Wavetable> (newTable));
            
            juce::Logger::writeToLog("Test " + juce::String(i) + " = " + juce::String(newTable.frameCount));
        }
        totalFilesLoaded++;
    }
    juce::Logger::writeToLog("Total Binary Files loaded: " + juce::String(totalFilesLoaded));
    numWavetables = totalFilesLoaded;
    
}

Wavetable WaveBankManager::parseBinaryData(juce::AudioBuffer<float> binaryData)
{
    int tableLength = 2048;
    
    int totalSamps = binaryData.getNumSamples();
    auto numTables = totalSamps / tableLength;
    
    //Create a new wavetable, and resize it
    Wavetable newTable;
    
    newTable.frameSize = tableLength;
    newTable.frameCount = numTables;
    
    newTable.frames.resize(numTables);
    for(auto& table : newTable.frames)
        table.resize(tableLength);
    
    newTable.mipmaps.resize(numTables);
    
    //Resize Vector for Raw Data array
    rawTables.resize(numTables);
    for(auto& table : rawTables)
        table.resize(tableLength);
    
    
    //Loop to copy data from binary to raw table array
    for(int i = 0; i < numTables; i++)
    {
        //Add datat to rawTable array
        auto pointer = rawTables[i].data();
        
        for(int j = 0; j < tableLength; j++)
        {
            pointer[j] = binaryData.getSample(0, i * tableLength + j);
        }
        
        //Add to wavetable array
        Wavetable table;
        table.frames.resize(tableLength);
        
        //Copy contigous data in parsed array
        for(int j = 0; j < tableLength; j++)
        {
            newTable.frames[i][j] = binaryData.getSample(0, i * tableLength + j);
        }
    }
    
    return newTable;
}

//---Generates Mip Maps Based on the sample rate----//
void WaveBankManager::generateMipmaps()
{
    
    if(verifySampleRate())
    {
        for(std::shared_ptr<Wavetable> wavetable : waveTables)
        {
            
            wavetable->mipmaps.resize(wavetable->frameCount);
            for(int i = 0; i < wavetable->frameCount; i++)
            {
                wavetable->mipmaps[i] = mipmapGenerator.generateMipMapStructs(wavetable->frames[i], sampleRate);
            }
        }
    }else
    {
        juce::Logger::writeToLog("Error Generating Mip Maps: Sample Rate Invalid");
    }
}

void WaveBankManager::updateSampleRate(double newSampleRate)
{
    sampleRate = newSampleRate;
}

bool WaveBankManager::verifySampleRate()
{
    return sampleRate == 48000.0 || sampleRate == 44100.0;
}

void WaveBankManager::prepareToPlay()
{
    if(verifySampleRate())
        generateMipmaps();
    else
        juce::Logger::writeToLog("Error in prepareToPlay for WaveBank Manager: invalid Sample Rate");
    
}


//Function to prepare Mipmaps for loading in
std::shared_ptr<const MipMap> WaveBankManager::formatMipMapForSynth(int bankID, int mapID)
{
    
    std::shared_ptr<const MipMap> map;
    
    if(bankID >= waveTables.size())
    {
        juce::Logger::writeToLog("Error formating MipMaps for Synthesizer, Bank ID out of range");
        return nullptr;
    }
        
    if(mapID >= waveTables[bankID]->frameCount)
    {
        juce::Logger::writeToLog("Error formating MipMaps for Synthesizer, Map ID out of range");
        return nullptr;
    }
    
    map = std::make_shared<MipMap>(waveTables[bankID]->mipmaps[mapID]);
    
    return map;
    
}

const char* WaveBankManager::getAssetData(AssetID id, size_t &size)
{
    switch (id)
        {
            case AssetID::Test_1:
                size = MyWavetableData::_01_RESO1_wavSize;
                return MyWavetableData::_01_RESO1_wav;

            case AssetID::Test_2:
                size = MyWavetableData::_02_RESO2_wavSize;
                return MyWavetableData::_02_RESO2_wav;

            case AssetID::Test_3:
                size = MyWavetableData::_03_MALET_wavSize;
                return MyWavetableData::_03_MALET_wav;

            default:
                size = 0;
                return nullptr;
        }
}

std::shared_ptr<const Wavetable> WaveBankManager::getWavetable(int index)
{
    if(0 <= index && index < numWavetables)
    {
        return waveTables[index];
    }
}

const std::vector<std::vector<float>>& WaveBankManager::getWavetableForDisplay(int wavetableID)
{
    jassert(wavetableID < numWavetables);
    
    return waveTables[wavetableID]->frames;
}

const std::vector<float>& WaveBankManager::getFrameForDisplay(int wavetableID, int frameID)
{
    jassert(wavetableID < waveTables.size());
    jassert(frameID < waveTables[wavetableID]->frameCount);
    
    return waveTables[wavetableID]->frames[frameID];
}

int WaveBankManager::getTotalWavetables()
{
    return numWavetables;
}



//------------------------------------------------------//
//----------Valid Debug Functions-----------------------//
//------------------------------------------------------//
void WaveBankManager::printTables(int tableNumber)
{

    float val = 0;
    
    std::shared_ptr<Wavetable> table = waveTables[tableNumber];
    
    for(int i = 0; i < table->frameSize; i++)
    {
        val = table->frames[0][i];
        juce::Logger::writeToLog("Wavetable " + juce::String(tableNumber) + juce::String(val));
        
    }
}



//Reports on the mip map data, and raw table data for a given wavetable and frame selection
void WaveBankManager::reportTableData(int tableNum, int frameNum)
{
    
    int numDataPoints = 50;
    
    std::shared_ptr<Wavetable> table = waveTables[tableNum];
    
    juce::Logger::writeToLog("Wavetable " + juce::String(tableNum) + ":");
    juce::Logger::writeToLog("  Frame " + juce::String(frameNum) + "Data:");
    
    double val;
    
    for(int i = 0; i < numDataPoints; i++)
    {
        val = table->frames[frameNum][i];
        juce::Logger::writeToLog(juce::String(i) + " -  " + juce::String(val));
        
    }
    
    juce::Logger::writeToLog("  MipMap " + juce::String(frameNum) + "Data:");
    
    const MipMap& mipmaps = table->mipmaps[frameNum];
    auto stage = mipmaps.getStage(0);
    
    for(int i = 0; i < numDataPoints; i++)
    {
        val = stage[i];
        juce::Logger::writeToLog(juce::String(i) + "-  " + juce::String(val));
        
    }
    
}


//------------------------------------------------------//
//Below is a general function, originally for loading any wavetable into waveBank
//------------------------------------------------------//

bool loadWavetable(juce::File wavetableFile, std::vector<std::vector<std::shared_ptr<const MipMap>>> waveBank, double sampleRate)
{
    int frameSize = 2048;
    
    MipMapGenerator m_mipmapGenerator;
    
    std::vector<std::shared_ptr<const MipMap>> tempBank;
    
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(wavetableFile));
    if (reader == nullptr) return 0;

    juce::AudioBuffer<float> buffer(1, (int)reader->lengthInSamples);
    reader->read(&buffer, 0, (int)reader->lengthInSamples, 0, true, false);
    
    int numFrames = buffer.getNumSamples() / frameSize;
    
    //For each frame, isolate wavetable, create mip map, and store in temporary Bank
    for( int i = 0; i < numFrames; ++i)
    {
        auto ptr = buffer.getReadPointer(0, i * frameSize);
        std::vector<float> frameBuffer(ptr, ptr + frameSize);
        
        auto mipMap = m_mipmapGenerator.generateMipMaps(frameBuffer, sampleRate);
        
        // loaded mipmap for current waveform
        auto mipmap = std::make_shared<MipMap>();
        mipmap->stages = std::move(mipMap);
        
        //Add Generated Mipmap to temp Wavebank to push_back later
        tempBank.push_back(mipmap);
    }
    
    //Add result to final waveBank
    waveBank.push_back(tempBank);

    return numFrames;
}


