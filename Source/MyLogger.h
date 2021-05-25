/*
  ==============================================================================

    MyLogger.h
    Created: 20 May 2021 3:38:48pm
    Author:  sflei_01

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class MyLogger
    : public juce::Logger
{

private:
    MyLogger();

    virtual ~MyLogger() override;

private:
    juce::FileLogger* _file_logger;
    bool _is_owner;

private:
    static MyLogger* _instance;

public:
    static MyLogger& instance();
    static void deleteInstance();

    static void log(const juce::String& message);

public:
    virtual void logMessage(const juce::String& message) override;

};
