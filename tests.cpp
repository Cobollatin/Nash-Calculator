#include  "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


namespace nash_calculator_tests
{
    BEGIN_TEST_MODULE_ATTRIBUTE()
        END_TEST_MODULE_ATTRIBUTE()

        TEST_MODULE_INITIALIZE(ModuleInitialize)
    {
        Logger::WriteMessage("In Module Initialize");
    }

    TEST_MODULE_CLEANUP(ModuleCleanup)
    {
        Logger::WriteMessage("In Module Cleanup");
    }

    TEST_CLASS(PureNashEquilibriumTests)
    {
    public:
        PureNashEquilibriumTests()
        {
            Logger::WriteMessage("In Class1");
        }

        ~PureNashEquilibriumTests() override
        {
            Logger::WriteMessage("In ~Class1");
        }

        TEST_CLASS_INITIALIZE(ClassInitialize)
        {
            Logger::WriteMessage("In Class Initialize");
        }

        TEST_CLASS_CLEANUP(ClassCleanup)
        {
            Logger::WriteMessage("In Class Cleanup");
        }

        BEGIN_TEST_METHOD_ATTRIBUTE(TestIncompleteFunction)
            TEST_OWNER(L"Cobollatin")
            TEST_PRIORITY(1)
            END_TEST_METHOD_ATTRIBUTE()

            TEST_METHOD(TestIncompleteFunction)
        {
            std::string result = "Hello";
            std::string expected = "Hello";
            Assert::AreEqual(expected, result);
        }
    };
}