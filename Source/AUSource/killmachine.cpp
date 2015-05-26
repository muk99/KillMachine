#include "AUEffectBase.h"
#include <AudioToolbox/AudioUnitUtilities.h>
#include "killmachineVersion.h"
#include <math.h>

//If you want to debug, please enter the path of the debug between ""
//#define DEBUG_PATH ""

#ifdef DEBUG_PATH
    #include <iostream>
    #include <fstream>
    #include <string>
#endif

using namespace std;

#pragma mark ____FilterKernel

class killmachineKernel : public AUKernelBase
{
public:
    killmachineKernel(AUEffectBase *inAudioUnit);
    virtual ~killmachineKernel();
    
    virtual void Process(   const Float32   *inSourceP,
                            Float32         *inDestP,
                            UInt32          inFramesToProcess,
                            UInt32          inNumChannels,
                            bool            &ioSilence );
    
    virtual void Reset();
    
private:
    long state;
};


#pragma mark ____Filter

class killmachine : public AUEffectBase
{
public:
    killmachine(AudioUnit component);
    
    virtual OSStatus Version() { return kFilterVersion; }
    
    virtual AUKernelBase *NewKernel() { return new killmachineKernel(this); }
    
    virtual OSStatus GetPropertyInfo(   AudioUnitPropertyID		inID,
                                        AudioUnitScope			inScope,
                                        AudioUnitElement		inElement,
                                        UInt32                  &outDataSize,
                                        Boolean                 &outWritable );
    
    virtual OSStatus GetProperty(       AudioUnitPropertyID 	inID,
                                        AudioUnitScope 			inScope,
                                        AudioUnitElement 		inElement,
                                        void 					*outData );
    
    
    virtual OSStatus GetParameterInfo(  AudioUnitScope			inScope,
                                        AudioUnitParameterID	inParameterID,
                                        AudioUnitParameterInfo	&outParameterInfo );
    
    virtual OSStatus GetPresets(CFArrayRef *outData)	const;
    virtual OSStatus NewFactoryPresetSet (const AUPreset &inNewFactoryPreset);
    
protected:
};


AUDIOCOMPONENT_ENTRY(AUBaseProcessFactory, killmachine)


enum
{
    kKillMachineParam_DeadLength = 0,
    kKillMachineParam_AliveLength = 1
};


static CFStringRef kDeadLength_Name = CFSTR("Dead Length");
static CFStringRef kAliveLength_Name = CFSTR("Alive Length");


const long kMinDeadLength       = 10;
const long kMaxDeadLength       = 441000;
const long kMinAliveLength      = 10;
const long kMaxAliveLength      = 441000;
const long kDefaultLength       = 1000;



// Factory presets
static const int kPreset_One = 0;
static const int kPreset_Two = 1;
static const int kNumberPresets = 2;

static AUPreset kPresets[kNumberPresets] =
{
    { kPreset_One, CFSTR("Preset One") },
    { kPreset_Two, CFSTR("Preset Two") }
};

static const int kPresetDefault = kPreset_One;
static const int kPresetDefaultIndex = 0;


#pragma mark ____Construction_Initialization

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// killmachine::killmachine
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
killmachine::killmachine(AudioUnit component)
: AUEffectBase(component)
{
    SetParameter(kKillMachineParam_DeadLength, kDefaultLength);
    SetParameter(kKillMachineParam_AliveLength, kDefaultLength);
    
    SetParamHasSampleRateDependency(false);
}


#pragma mark ____Parameters

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	Filter::GetParameterInfo
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus killmachine::GetParameterInfo( AudioUnitScope			inScope,
                                        AudioUnitParameterID	inParameterID,
                                        AudioUnitParameterInfo	&outParameterInfo )
{
    OSStatus result = noErr;
    
    outParameterInfo.flags =
        kAudioUnitParameterFlag_IsWritable + kAudioUnitParameterFlag_IsReadable;
    
    if (inScope == kAudioUnitScope_Global)
    {
        switch(inParameterID)
        {
            case kKillMachineParam_DeadLength:
                AUBase::FillInParameterName (outParameterInfo, kDeadLength_Name, false);
                outParameterInfo.unit = kAudioUnitParameterUnit_Indexed;
                outParameterInfo.minValue = kMinDeadLength;
                outParameterInfo.maxValue = kMaxDeadLength;
                outParameterInfo.defaultValue = kDefaultLength;
                outParameterInfo.flags += kAudioUnitParameterFlag_IsHighResolution;
                outParameterInfo.flags += kAudioUnitParameterFlag_DisplayLogarithmic;
                break;
                
            case kKillMachineParam_AliveLength:
                AUBase::FillInParameterName (outParameterInfo, kAliveLength_Name, false);
                outParameterInfo.unit = kAudioUnitParameterUnit_Indexed;
                outParameterInfo.minValue = kMinAliveLength;
                outParameterInfo.maxValue = kMaxAliveLength;
                outParameterInfo.defaultValue = kDefaultLength;
                outParameterInfo.flags += kAudioUnitParameterFlag_IsHighResolution;
                outParameterInfo.flags += kAudioUnitParameterFlag_DisplayLogarithmic;
                break;
                
            default:
                result = kAudioUnitErr_InvalidParameter;
                break;
        }
    }
    else
    {
        result = kAudioUnitErr_InvalidParameter;
    }
    
    return result;
}


#pragma mark ____Properties

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	killmachine::GetPropertyInfo
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus killmachine::GetPropertyInfo(	AudioUnitPropertyID     inID,
                                        AudioUnitScope          inScope,
                                        AudioUnitElement        inElement,
                                        UInt32                  &outDataSize,
                                        Boolean                 &outWritable)

{
    return AUEffectBase::GetPropertyInfo (inID, inScope, inElement, outDataSize, outWritable);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	Filter::GetProperty
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus killmachine::GetProperty(      AudioUnitPropertyID 	inID,
                                        AudioUnitScope 			inScope,
                                        AudioUnitElement		inElement,
                                        void                    *outData)
{
    return AUEffectBase::GetProperty (inID, inScope, inElement, outData);
}


#pragma mark ____Presets

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	Filter::GetPresets
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus killmachine::GetPresets(CFArrayRef *outData) const
{
    if (outData == NULL) return noErr;
    
    CFMutableArrayRef theArray = CFArrayCreateMutable (NULL, kNumberPresets, NULL);
    for (int i = 0; i < kNumberPresets; ++i) {
        CFArrayAppendValue (theArray, &kPresets[i]);
    }
    
    *outData = (CFArrayRef)theArray;
    return noErr;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	Filter::NewFactoryPresetSet
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
OSStatus killmachine::NewFactoryPresetSet(const AUPreset &inNewFactoryPreset)
{
    SInt32 chosenPreset = inNewFactoryPreset.presetNumber;
    
    for(int i = 0; i < kNumberPresets; ++i)
    {
        if(chosenPreset == kPresets[i].presetNumber)
        {
            switch(chosenPreset)
            {
                case kPreset_One:
                    SetParameter(kKillMachineParam_DeadLength, 1000 );
                    SetParameter(kKillMachineParam_AliveLength, 100 );
                    break;
                case kPreset_Two:
                    SetParameter(kKillMachineParam_DeadLength, 5000 );
                    SetParameter(kKillMachineParam_AliveLength, 1000 );
                    break;
            }
            
            SetAFactoryPresetAsCurrent (kPresets[i]);
            return noErr;
        }
    }
    
    return kAudioUnitErr_InvalidPropertyValue;
}

#pragma mark ____killmachineKernel

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	FilterKernel::FilterKernel()
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
killmachineKernel::killmachineKernel(AUEffectBase *inAudioUnit)
: AUKernelBase(inAudioUnit)
{
    Reset();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	FilterKernel::~FilterKernel()
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
killmachineKernel::~killmachineKernel( )
{
    
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	FilterKernel::Reset()
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void killmachineKernel::Reset()
{
    state = 0;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//FilterKernel::Process(int inFramesToProcess)
//
//		We process one non-interleaved stream at a time
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void killmachineKernel::Process(	const Float32 	*inSourceP,
                                    Float32 		*inDestP,
                                    UInt32 			inFramesToProcess,
                                    UInt32			inNumChannels, // for version 2 AudioUnits inNumChannels is always 1
                                    bool            &ioSilence)
{
    long dLength = GetParameter(kKillMachineParam_DeadLength);
    long aLength = GetParameter(kKillMachineParam_AliveLength);
    
    const Float32 *sourceP = inSourceP;
    int n = inFramesToProcess;
    
    Float32 *destP = inDestP,
            input_sample,
            output_sample;
    
 
    // debug
    #ifdef DEBUG_PATH
    std::ofstream debug(DEBUG_PATH, std::ios::out | std::ios::app);
    #endif

    while(n--)
    {
        // length state up
        state++;
        if(state >= dLength + aLength) state = 0;
        
        // get sample
        input_sample = *sourceP;
        
        #ifdef DEBUG_PATH
        debug << "length = " << dLength + aLength << "\t" << state;
        #endif
        
        //dead or alive
        if(state < dLength)
        {
            output_sample = 0;
            #ifdef DEBUG_PATH
            debug << "\tON" <<std::endl;
            #endif
        }
        else
        {
            output_sample = input_sample;
            #ifdef DEBUG_PATH
            debug << "\tOFF" <<std::endl;
            #endif
        }
        
        // output
        *destP = output_sample;
        
        //next pointer
        sourceP++;
        destP++;
    }
    
}