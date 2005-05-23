/**
 *  Config Handler for Lincity-NG.
 *   
 *  March 2005, Wolfgang Becker <uafr@gmx.de>
 *
 */
#include <config.h>

#include "Config.hpp"
#include "gui/XmlReader.hpp"
#include "PhysfsStream/PhysfsStream.hpp"

#include <assert.h>
#include <iostream>

Config* configPtr = 0;

Config *getConfig()
{
    if(configPtr == 0)
        configPtr = new Config();

    return configPtr;
}

Config::Config()
{
    assert(configPtr == 0);
    
    //Default Values
    useOpenGL = false;
    useFullScreen = true;
    videoX = 800; 
    videoY = 600;

    soundVolume = 100;
    musicVolume = 50;
    soundEnabled = true;
    musicEnabled = true;

    skipMonthsFast = 1;
    upgradeTransport = true;
    //First we load the global File which should contain
    try {
        //sane defaults for the local system.
        load( "lincityconfig.xml" );
    } catch(std::exception& e) {
#ifdef DEBUG
        std::cerr << "Couldn't load lincityconfig.xml: " << e.what() << "\n";
#endif
    }
    //Then load another file where to overwrite some values.
    try {
        load( "userconfig.xml" );
    } catch(std::exception& e) {
#ifdef DEBUG
        std::cerr << "Couldn't load userconfig.xml: " << e.what() << "\n";
#endif
    }
}

Config::~Config()
{
    if( configPtr == this )
    {
        configPtr = 0;
    }
}

/*
 * Read Integer Value from char-Array.
 * use defaultValue on Errors or if Value is not in given Interval.
 */
int Config::parseInt( const char* value, int defaultValue, int minValue, int maxValue ) {
    int tmp; 
    if(sscanf(value, "%i", &tmp) != 1) 
    {
        std::cerr << "Config::parseInt# Error parsing integer value '" << value << "'.\n";
        tmp = defaultValue;
    }
    if( ( tmp >= minValue ) && ( tmp <= maxValue ) ) {
        return tmp;
    } else {
        std::cerr << "Config::parseInt# Value '" << value << "' not in ";
        std::cerr << minValue << ".." << maxValue << "\n";
        return defaultValue;
    }
}
/*
 * Load configuration from File.
 */
void Config::load( const std::string& filename ){
    XmlReader reader( filename );
    while( reader.read() ) {
        if( reader.getNodeType() == XML_READER_TYPE_ELEMENT) 
        {
            const std::string& element = (const char*) reader.getName();
            if( element == "video" ) {
                XmlReader::AttributeIterator iter(reader);
                while(iter.next()) 
                {
                    const char* name = (const char*) iter.getName();
                    const char* value = (const char*) iter.getValue();
                    if( strcmp( name, "useOpenGL" ) == 0 ) {
                        useOpenGL  = parseBool(value, false);
                    } else if( strcmp(name, "x" ) == 0 ) {
                        videoX = parseInt( value, 800, 640 );
                    } else if(strcmp(name, "y") == 0 ) {
                        videoY = parseInt( value, 600, 480 );
                    } else if(strcmp(name, "fullscreen") == 0) {
                        useFullScreen = parseBool(value, false);
                    } else {
                        std::cerr << "Config::load# Unknown attribute '" << name;
                        std::cerr << "' in element '" << element << "' from " << filename << ".\n";
                    }
                }
            } else if ( element == "audio" ) {
                XmlReader::AttributeIterator iter(reader);
                while(iter.next()) 
                {
                    const char* name = (const char*) iter.getName();
                    const char* value = (const char*) iter.getValue();
                    if(strcmp(name, "soundVolume" ) == 0) {
                        soundVolume = parseInt(value, 100, 0, 100);
                    } else if(strcmp(name, "musicVolume") == 0) {
                        musicVolume = parseInt(value, 100, 0, 100);
                    } else if(strcmp(name, "soundEnabled") == 0) {
                        soundEnabled = parseBool(value, true);
                    } else if(strcmp(name, "musicEnabled") == 0) {
                        musicEnabled = parseBool(value, true);
                    } else {
                        std::cerr << "Config::load# Unknown attribute '" << name;
                        std::cerr << "' in element '" << element << "' from " << filename << ".\n";
                    }
                }
            } else if ( element == "game" ) {
                XmlReader::AttributeIterator iter(reader);
                while(iter.next()) 
                {
                    const char* name = (const char*) iter.getName();
                    const char* value = (const char*) iter.getValue();
                    if(strcmp(name, "skipMonthsFast" ) == 0) {
                        skipMonthsFast = parseInt( value, 1, 1 );
                    } else if( strcmp(name, "upgradeTransport" ) == 0 ){
                       upgradeTransport  = parseBool(value, true);
                    } else {
                        std::cerr << "Config::load# Unknown attribute '" << name;
                        std::cerr << "' in element '" << element << "' from " << filename << ".\n";
                    }
                }
            } else {
                std::cerr << "Config::load# Unknown element '" << element << "' in "<< filename << ".\n";
            }
        }
    }
}

bool
Config::parseBool(const char* value, bool defaultValue)
{
    if(strcmp(value, "no") == 0 || strcmp(value, "off") == 0
            || strcmp(value, "false") == 0 || strcmp(value, "NO") == 0
            || strcmp(value, "OFF") == 0 || strcmp(value, "FALSE") == 0) {
        return false;
    }
    if(strcmp(value, "yes") == 0 || strcmp(value, "on") == 0
            || strcmp(value, "true") == 0 || strcmp(value, "YES") == 0
            || strcmp(value, "ON") == 0 || strcmp(value, "TRUE") == 0) {
        return true;
    }

    std::cerr << "Couldn't parse boolean value '" << value << "'.\n";
    return defaultValue;
}

/*
 * Save configuration to File.
 */
void
Config::save(){
    OFileStream userconfig( "userconfig.xml" );
    userconfig << "<?xml version=\"1.0\"?>\n";
    userconfig << "<configuration>\n";
    userconfig << "    <video x=\"" << videoX << "\" y=\"" << videoY << "\" useOpenGL=\"" 
        << (useOpenGL?"yes":"no") << "\" fullscreen=\"" << (useFullScreen?"yes":"no")  
        << "\" />\n";
    userconfig << "    <audio soundEnabled=\"" << (soundEnabled?"yes":"no")  
        << "\" soundVolume=\"" << soundVolume << "\" \n";
    userconfig << "           musicEnabled=\"" << (musicEnabled?"yes":"no")  
        << "\" musicVolume=\"" << musicVolume << "\" />\n";
    userconfig << "</configuration>\n";
}

