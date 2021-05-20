/*
  ==============================================================================

    ComponentAttachmentWrapper.h
    Created: 20 May 2021 3:38:06pm
    Author:  sflei_01

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class AbstractComponentAttachmentWrapper
{
public:
    virtual ~AbstractComponentAttachmentWrapper()
    {
    }
};

template <class TComponentAttachment>
class ComponentAttachmentWrapper
    : public AbstractComponentAttachmentWrapper
{
public:
    ComponentAttachmentWrapper()
        : _component_attachment_ptr()
    {
    }

    explicit ComponentAttachmentWrapper(std::unique_ptr<TComponentAttachment> component_attachment_ptr)
        : _component_attachment_ptr( std::move( component_attachment_ptr ) )
    {
    }

    virtual ~ComponentAttachmentWrapper() override
    {
        _component_attachment_ptr.reset();
    }

private:
    std::unique_ptr<TComponentAttachment> _component_attachment_ptr;
};
