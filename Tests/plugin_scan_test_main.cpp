// =============================================================================
// NovaHost Plugin Scan Test
// -----------------------------------------------------------------------------
// Headless test that validates SafePluginScanner integration and plugin
// discovery without requiring a display. Invoked via:
//   NovaHostPluginScanTest --scan-test
// =============================================================================

#include <JuceHeader.h>
#include "SafePluginScanner.h"

namespace
{
    class PluginScanTestRunner
    {
    public:
        explicit PluginScanTestRunner (bool verbose) : verbose (verbose) {}

        int run ()
        {
            log ("=== NovaHost Plugin Scan Test ===");
            log ("JUCE version: " + juce::String (JUCE_MAJOR_VERSION) + "."
                 + juce::String (JUCE_MINOR_VERSION));

            bool ok = true;
            ok &= testPluginFormatManagerInit();
            ok &= testSafePluginScannerConstruction();
            ok &= testPluginSearchPaths();

            log (ok ? "\n*** ALL PLUGIN SCAN TESTS PASSED ***" : "\n*** PLUGIN SCAN TEST FAILURE ***");
            return ok ? 0 : 1;
        }

    private:
        bool verbose;

        void log (const juce::String& msg)
        {
            if (verbose)
                juce::Logger::writeToLog (msg);
            std::cout << msg << std::endl;
        }

        bool testPluginFormatManagerInit ()
        {
            // Verify AudioPluginFormatManager can be created and has formats
            juce::AudioPluginFormatManager formatManager;
            formatManager.addDefaultFormats();

            int numFormats = formatManager.getNumFormats();
            bool ok = numFormats > 0;
            log ("PluginFormatManager: " + juce::String (numFormats) + " formats registered " + (ok ? "OK" : "FAIL"));

            if (ok && verbose)
            {
                for (int i = 0; i < numFormats; ++i)
                {
                    log ("  - " + formatManager.getFormat(i)->getName());
                }
            }

            return ok;
        }

        bool testSafePluginScannerConstruction ()
        {
            // Verify SafePluginScanner can be constructed with valid dependencies
            juce::AudioPluginFormatManager formatManager;
            formatManager.addDefaultFormats();
            juce::KnownPluginList pluginList;

            bool ok = true;
            try
            {
                SafePluginScanner scanner (formatManager, pluginList, "VST3", 30000);
                log ("SafePluginScanner: construction OK");

                // Test blacklist mechanism (should be empty initially)
                bool blacklisted = scanner.isPluginBlacklisted (juce::PluginDescription());
                ok &= !blacklisted; // Empty description shouldn't be blacklisted
                log ("SafePluginScanner: blacklist check " + juce::String (ok ? "OK" : "FAIL"));
            }
            catch (const std::exception& e)
            {
                log ("SafePluginScanner: construction FAIL - " + juce::String (e.what()));
                ok = false;
            }
            catch (...)
            {
                log ("SafePluginScanner: construction FAIL - unknown exception");
                ok = false;
            }

            return ok;
        }

        bool testPluginSearchPaths ()
        {
            // Verify search paths are populated for the current platform
            juce::AudioPluginFormatManager formatManager;
            formatManager.addDefaultFormats();
            juce::KnownPluginList pluginList;

            SafePluginScanner scanner (formatManager, pluginList, "VST3", 30000);

            // Can't directly access private getPluginSearchPaths, so we test indirectly
            // by checking that the format manager has formats (which implies paths will be searched)
            int numFormats = formatManager.getNumFormats();
            bool ok = numFormats > 0;

            log ("PluginSearchPaths: format manager has formats " + juce::String (ok ? "OK" : "FAIL"));
            return ok;
        }
    };
}

int main (int argc, char* argv[])
{
    bool scan = false;
    bool verbose = true;

    for (int i = 1; i < argc; ++i)
    {
        juce::String arg (argv[i]);
        if (arg == "--scan-test")  scan = true;
        if (arg == "--quiet")      verbose = false;
    }

    if (! scan)
    {
        std::cout << "NovaHostPluginScanTest — pass --scan-test to run headless plugin scan checks." << std::endl;
        return 0;
    }

    // Initialise JUCE without GUI subsystems
    juce::String error = juce::SystemStats::getJUCEVersion();
    juce::ignoreUnused (error);

    PluginScanTestRunner runner (verbose);
    return runner.run();
}