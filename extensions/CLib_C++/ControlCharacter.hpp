#ifndef CLIB_CONTROL_CHARACTER
#define CLIB_CONTROL_CHARACTER

namespace CLib
{
    namespace ControlCharacter
    {
		/**
         * Character marking the start of the header region
         */
        const char SOH = '\x01';
        /**
         * Character marking the start of the text region
         */
        const char STX = '\x02';
        /**
         * Character marking the end of the text region
         */
        const char ETX = '\x03';
        const char EOT = '\x04';
        const char ENQ = '\x05';
        const char ACK = '\x06';

        const char RS = '\x1E';
        /**
         * Separating character for header values
         */
        const char US = '\x1F';
    } // namespace ControlCharacter
} // namespace CLib

#endif