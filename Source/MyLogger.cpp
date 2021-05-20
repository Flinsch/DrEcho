/*
  ==============================================================================

    MyLogger.cpp
    Created: 20 May 2021 3:38:48pm
    Author:  sflei_01

  ==============================================================================
*/

#include "MyLogger.h"

MyLogger* MyLogger::_instance = nullptr;

MyLogger::MyLogger()
    : _file_logger( nullptr )
    , _is_owner( false )
{
    juce::String logFileSubDirectoryName( "" );
    juce::String logFileName( JucePlugin_Name + juce::String("-log.txt") );
    juce::String welcomeMessage( "" );
    juce::int64 maxInitialFileSizeBytes = 0;

    _file_logger = juce::FileLogger::createDefaultAppLogger( logFileSubDirectoryName, logFileName, welcomeMessage, maxInitialFileSizeBytes );
    if ( !_file_logger )
    {
        //_file_logger = new juce::FileLogger( ... );
        _is_owner = true;
    }
}

MyLogger::~MyLogger()
{
    if ( _file_logger && _is_owner )
        delete _file_logger;
}

MyLogger& MyLogger::instance()
{
    if ( !_instance )
        _instance = new MyLogger();
    return *_instance;
}

void MyLogger::deleteInstance()
{
    if ( _instance )
        delete _instance;
    _instance = nullptr;
}

void MyLogger::log(const juce::String& message)
{
    instance().logMessage( message );
}

void MyLogger::logMessage(const juce::String& message)
{
    if ( !_file_logger )
        return;

    _file_logger->logMessage( message );
}
