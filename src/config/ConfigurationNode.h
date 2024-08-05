
#ifndef CONFIGURATIONNODE_H
#define CONFIGURATIONNODE_H

#include "ComplexField.h"
#include "ConfigParser.h"
#include "SimpleField.h"
#include "TypeConverter.h"

#include <iostream>
#include <cstddef>
#include <string>

class ConfigurationNode
{
public:
    virtual ~ConfigurationNode() {}

    void addChild(std::string const& name, ConfigurationNode* child)
    {
        if (childExists(name))
        {
            child->init(_content);
        }
        else
        {
            if (child->isMandatory())
            {
                std::cout << "Could not find [" << name << "], which is a mandatory field \n";
                abort();
            }
            else
            {
                child = NULL;
            }
        }
    }

    virtual void init(std::string& parentContent) = 0;

    void setMandatory(bool const& mandatory)
    {
        _mandatory = mandatory;
    }

    void setUnique(bool const& unique)
    {
        _unique = unique;
    }

    bool isMandatory() const
    {
        return _mandatory;
    }

    bool isUnique() const
    {
        return _unique;
    }

protected:
    std::string::size_type setContent(std::string const& parentContent, std::string const& childName)
    {
        std::string::size_type contentPosition = parentContent.find(childName);
        _content = parentContent.substr(contentPosition);

        return contentPosition;
    }

    std::string::size_type cutContentsEnd()
    {
        std::string::size_type firstOpenBracketPosition = _content.find('{');
        std::string::size_type firstCloseBracketPosition = _content.find('}');
        std::string::size_type finalPosition = std::string::npos;

        if (firstCloseBracketPosition < firstOpenBracketPosition)
        {
            std::cout << "something went wrong parsing the following configuration section: \n" <<
                _content << "\n";

            abort();
        }

        std::string inspectString = _content.substr(firstOpenBracketPosition + 1);
        int missingClosedBrackets = 1;
        finalPosition = firstOpenBracketPosition;

        do
        {
            std::string::size_type openBracketPosition = inspectString.find('{');
            std::string::size_type closeBracketPosition = inspectString.find('}');

            if (closeBracketPosition < openBracketPosition)
            {
                missingClosedBrackets--;
                finalPosition += closeBracketPosition;

                inspectString = inspectString.substr(closeBracketPosition + 1);
            }
            else
            {
                missingClosedBrackets++;
                finalPosition += openBracketPosition;
                inspectString = inspectString.substr(openBracketPosition + 1);
            }

        } while (missingClosedBrackets);

        _content = _content.substr(0, finalPosition);

        return finalPosition;
    }

    template <class T>
    void bind(std::string const& varName, FieldInterface<T>* field, bool isMandatory)
    {
        field->setSourceString(ConfigParser::getVariable(varName, _content, isMandatory));
    }

    virtual void actuallyInit() = 0;

    bool childExists(std::string const& childName)
    {
        std::string::size_type position = _content.find(childName);

        return position != std::string::npos;
    }

    std::string _content;

private:
    bool _mandatory;
    bool _unique;
};

#endif
