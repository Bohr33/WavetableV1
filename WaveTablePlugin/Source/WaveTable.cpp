/*
  ==============================================================================

    WaveTable.cpp
    Created: 26 Dec 2025 9:39:19pm
    Author:  Benjamin Ward (Old Computer)

  ==============================================================================
*/

#include "WaveTable.h"


void WaveBankManager::loadWavetableFromBinary()
{
    
    //Must make a unique pointer for the createReaderFor function
    
    //Re work function to be abstract, currently hardcoded, should work for all new additions, probably use an outside loop to call this function with hard coded enums
    auto memStream = std::make_unique<juce::MemoryInputStream>(
                    MyWavetableData::Virus_1_wav,
                    MyWavetableData::Virus_1_wavSize,
                    false
                                                               );
    
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();  // WAV, AIFF, MP3 (if enabled)

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(std::move(memStream)));
    
    if (reader != nullptr)
    {
        // Create an AudioBuffer with the right number of channels and samples
        juce::AudioBuffer<float> buffer(reader->numChannels, static_cast<int>(reader->lengthInSamples));

        // Read all samples into the buffer
        reader->read(&buffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
        
        
        //Parse file, need to take buffer, split it up into wavetables and store them in the wavebank
        parseBinaryData(buffer);
    }
}

void WaveBankManager::parseBinaryData(juce::AudioBuffer<float> binaryData)
{
    int tableLength = 2048;
    
    
    int totalSamps = binaryData.getNumSamples();
    auto numTables = totalSamps / tableLength;
    
    
    //Create a new wavetable, and resize it
    Wavetable newTable;
    
    newTable.frameSize = tableLength;
    newTable.frameCount = numTables;
    
    newTable.name = "Virus1";
    
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
        
        //Add to wavetable array, with names and such
        Wavetable table;
        table.frames.resize(tableLength);
        table.name = "Virus1 " + juce::String(i);
        
        //Copy contigous data in parsed array
        for(int j = 0; j < tableLength; j++)
        {
            newTable.frames[i][j] = binaryData.getSample(0, i * tableLength + j);
        }
        
    }
    
    
    waveTables.push_back(newTable);
}

//---Generates Mip Maps Based on the sample rate----//
void WaveBankManager::generateMipmaps()
{
    
    if(verifySampleRate())
    {
        for(auto& wavetable : waveTables)
        {
            for(int i = 0; i < wavetable.frameCount; i++)
            {
                wavetable.mipmaps[i] = mipmapGenerator.generateMipMapStructs(wavetable.frames[i], sampleRate);
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



//------------------------------------------------------//
//----------Valid Debug Functions-----------------------//
//------------------------------------------------------//
void WaveBankManager::printTables(int tableNumber)
{

    float val = 0;
    
    Wavetable table = waveTables[tableNumber];
    
    for(int i = 0; i < table.frameSize; i++)
    {
        val = table.frames[0][i];
        juce::Logger::writeToLog("Wavetable " + juce::String(tableNumber) + juce::String(val));
        
    }
}



//Reports on the mip map data, and raw table data for a given wavetable and frame selection
void WaveBankManager::reportTableData(int tableNum, int frameNum)
{
    
    int numDataPoints = 50;
    
    Wavetable table = waveTables[tableNum];
    
    juce::Logger::writeToLog("Wavetable " + juce::String(tableNum) + ":");
    juce::Logger::writeToLog("  Frame " + juce::String(frameNum) + "Data:");
    
    double val;
    
    for(int i = 0; i < numDataPoints; i++)
    {
        val = table.frames[frameNum][i];
        juce::Logger::writeToLog(juce::String(i) + " -  " + juce::String(val));
        
    }
    
    juce::Logger::writeToLog("  MipMap " + juce::String(frameNum) + "Data:");
    
    const MipMap& mipmaps = table.mipmaps[frameNum];
    auto stage = mipmaps.getStage(0);
    
    for(int i = 0; i < numDataPoints; i++)
    {
        val = stage[i];
        juce::Logger::writeToLog(juce::String(i) + "-  " + juce::String(val));
        
    }
    
}








//------------------------------------------------------//
//-----------Break to Highlight New functions-----------//
//------------------------------------------------------//


//void WaveBankManager::loadDefaultTables(const juce::File& rescourceFolder)
//{
//
//    juce::Array<juce::File> files;
//
//    //THis is the issue here, find child FIles is not working, and I think its because
//    // we need to load as binary data.
//
//
//    files = rescourceFolder.findChildFiles(
//           juce::File::findFiles,
//           true,          // search subfolders
//           "*.wav");
//
//
//
//
//
//    juce::Logger::writeToLog("Test Test Test");
//
//    if(files.size() == 0)
//    {
//        juce::Logger::writeToLog("Error Loading Rescource Files");
//        return 1;
//    }
//
//    for (int i = 0; i < files.size(); ++i)
//    {
//        juce::Logger::writeToLog(files[i].getFullPathName());
//    }
//
//       for (auto& file : files)
//       {
//           auto table = loadWaveTableFile(file);
//           waveTables.push_back(table);
//
//           juce::Logger::writeToLog("Loaded wavetable: " + file.getFullPathName());
//       }
//
//}

//
//Wavetable WaveBankManager::loadWaveTableFile(juce::File file)
//{
//
//    Wavetable table;
//    table.name = file.getFileNameWithoutExtension();
//
//    juce::AudioFormatManager formatManager;
//    formatManager.registerBasicFormats();
//
//    std::unique_ptr<juce::AudioFormatReader> reader(
//        formatManager.createReaderFor(file));
//
//    if (reader == nullptr)
//    {
//        juce::Logger::writeToLog("Failed to load wavetable: " + file.getFullPathName());
//        return table;
//    }
//
//    const int totalSamples = static_cast<int>(reader->lengthInSamples);
//    const int frameSize = 2048; // common wavetable size
//
//    int frameCount = totalSamples / frameSize;
//
//    if (frameCount == 0)
//    {
//        juce::Logger::writeToLog("Invalid wavetable size: " + file.getFileName());
//        return table;
//    }
//
//    table.frameSize = frameSize;
//    table.frameCount = frameCount;
//
//    juce::AudioBuffer<float> buffer(1, totalSamples);
//
//    reader->read(&buffer,
//                 0,
//                 totalSamples,
//                 0,
//                 true,
//                 false);
//
//    for (int i = 0; i < totalSamples; i++)
//    {
//        table.frames[i] = buffer.getSample(0, i);
//    }
//
//    juce::Logger::writeToLog(
//        "Loaded wavetable: " + table.name +
//        " (" + juce::String(frameCount) + " frames)");
//
//    return table;
//
//}

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


