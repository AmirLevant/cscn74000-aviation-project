#include "pch.h"
#include "CppUnitTest.h"
#include "client.h"
#include "helper_functions.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace cscn74000aviationprojectclienttests
{
    TEST_CLASS(Packet_Tests)
    {
    public:
        TEST_METHOD(Constructor_Telemetry_InitializesFieldsCorrectly)
        {
            uint8_t testData[] = { 1, 2, 3, 4 };
            uint8_t* data = new uint8_t[4];
            memcpy(data, testData, 4);
            uint32_t sendId = 123;
            uint32_t destId = 456;
            uint32_t transNum = 789;
            uint16_t bodyLen = 4;
            Packet packet(sendId, destId, transNum, bodyLen, data);

            Assert::AreEqual(sendId, packet.getSenderId());
            Assert::AreEqual(destId, packet.getDestinationId());
            Assert::AreEqual(transNum, packet.getTransactionNum());
            Assert::AreEqual(bodyLen, packet.getBodyLength());
            Assert::AreEqual((int)InteractionType::Telemetry, (int)packet.getInteractionType());
            Assert::AreEqual((int)RequestType::Non_Request, (int)packet.getRequestType());

            uint8_t* body = packet.getBody();
            for (int i = 0; i < bodyLen; i++) {
                Assert::AreEqual(testData[i], body[i]);
            }
        }

        TEST_METHOD(Constructor_Request_InitializesFieldsCorrectly)
        {
            uint8_t testData[] = { 5, 6 };
            uint8_t* data = new uint8_t[2];
            memcpy(data, testData, 2);
            RequestType reqType = RequestType::Non_Request;
            uint32_t sendId = 111;
            uint32_t destId = 222;
            uint32_t transNum = 333;
            uint16_t bodyLen = 2;
            Packet packet(sendId, destId, reqType, transNum, bodyLen, data);

            Assert::AreEqual(sendId, packet.getSenderId());
            Assert::AreEqual(destId, packet.getDestinationId());
            Assert::AreEqual(transNum, packet.getTransactionNum());
            Assert::AreEqual(bodyLen, packet.getBodyLength());
            Assert::AreEqual((int)InteractionType::Request, (int)packet.getInteractionType());
            Assert::AreEqual((int)reqType, (int)packet.getRequestType());

            uint8_t* body = packet.getBody();
            for (int i = 0; i < bodyLen; i++) {
                Assert::AreEqual(testData[i], body[i]);
            }
        }

        TEST_METHOD(Constructor_FromBuffer_CorrectlyDeserializes)
        {
            uint8_t testData[] = { 1, 2, 3 };
            uint8_t* data = new uint8_t[3];
            memcpy(data, testData, 3);
            uint32_t sendId = 123;
            uint32_t destId = 456;
            uint32_t transNum = 789;
            uint16_t bodyLen = 3;
            Packet originalPacket(sendId, destId, transNum, bodyLen, data);

            uint32_t packetSize = originalPacket.get_packetSize();
            uint8_t* buffer = new uint8_t[packetSize];
            originalPacket.Serialize(buffer);

            Packet deserializedPacket(buffer);

            Assert::AreEqual(originalPacket.get_timestamp(), deserializedPacket.get_timestamp());
            Assert::AreEqual(originalPacket.getSenderId(), deserializedPacket.getSenderId());
            Assert::AreEqual(originalPacket.getDestinationId(), deserializedPacket.getDestinationId());
            Assert::AreEqual((int)originalPacket.getInteractionType(), (int)deserializedPacket.getInteractionType());
            Assert::AreEqual((int)originalPacket.getRequestType(), (int)deserializedPacket.getRequestType());
            Assert::AreEqual(originalPacket.getTransactionNum(), deserializedPacket.getTransactionNum());
            Assert::AreEqual(originalPacket.getBodyLength(), deserializedPacket.getBodyLength());

            uint8_t* originalBody = originalPacket.getBody();
            uint8_t* deserializedBody = deserializedPacket.getBody();
            for (int i = 0; i < bodyLen; i++) {
                Assert::AreEqual(originalBody[i], deserializedBody[i]);
            }

            delete[] buffer;
        }

        TEST_METHOD(ConvertToAckPacket_SwapsSenderAndDestination)
        {
            uint32_t sendId = 123;
            uint32_t destId = 456;
            Packet packet(sendId, destId, 0, 0, nullptr);

            packet.convertToAckPacket();

            Assert::AreEqual(destId, packet.getSenderId());
            Assert::AreEqual(sendId, packet.getDestinationId());
            Assert::AreEqual((int)InteractionType::Response, (int)packet.getInteractionType());
            Assert::AreEqual((int)RequestType::Non_Request, (int)packet.getRequestType());
            Assert::AreEqual(0, (int)packet.getBodyLength());
            Assert::IsNull(packet.getBody());
        }

        TEST_METHOD(GetPacketSize_ReturnsHeaderSizePlusBodyLength)
        {
            uint8_t testData[10];
            uint8_t* data = new uint8_t[10];
            memcpy(data, testData, 10);
            Packet packet(0, 0, 0, 10, data);

            uint32_t expectedSize = PACKET_HEADER_SIZE + 10;
            Assert::AreEqual(expectedSize, packet.get_packetSize());
        }

        TEST_METHOD(Checksum_ChangesAfterModification)
        {
            uint8_t* data = new uint8_t[2]{ 0x01, 0x02 };
            Packet originalPacket(1, 0, 0, 2, data);
            uint32_t originalSize = originalPacket.get_packetSize();
            uint8_t* originalBuffer = new uint8_t[originalSize];
            originalPacket.Serialize(originalBuffer);
            uint16_t originalChecksum;
            memcpy(&originalChecksum, originalBuffer + 28, 2);

            data = new uint8_t[2]{ 0x01, 0x02 };
            Packet modifiedPacket(1, 0, 0, 2, data);
            modifiedPacket.convertToAckPacket();
            uint32_t newSize = modifiedPacket.get_packetSize();
            uint8_t* newBuffer = new uint8_t[newSize];
            modifiedPacket.Serialize(newBuffer);
            uint16_t newChecksum;
            memcpy(&newChecksum, newBuffer + 28, 2);

            Assert::AreNotEqual(originalChecksum, newChecksum);

            delete[] originalBuffer;
            delete[] newBuffer;
        }
    };

    TEST_CLASS(Plane_Tests)
    {
    public:
        TEST_METHOD(Constructor_WithID_InitializesDefaults)
        {
            Plane plane(123);

            Assert::AreEqual(350u, plane.getDistanceFromGround());
            Assert::AreEqual(0u, plane.getCurrentTransactionNum());
            Assert::IsTrue(plane.getGoNoGo() == Go_NoGo::Go);
        }

        TEST_METHOD(Constructor_WithParameters_SetsValues)
        {
            Plane plane(123, Carrier::SunWing, Country::Armenia, Model::Boeing_787_Dreamliner);

            Assert::AreEqual(123u, plane.getId());
            Assert::IsTrue(plane.getCarrier() == Carrier::SunWing);
            Assert::IsTrue(plane.getCountry() == Country::Armenia);
            Assert::IsTrue(plane.getModel() == Model::Boeing_787_Dreamliner);
            Assert::AreEqual(350u, plane.getDistanceFromGround());
        }

        TEST_METHOD(Constructor_FromBuffer_CorrectDeserialization)
        {
            // Create original plane and serialize
            Plane original(123, Carrier::Virgin, Country::Canada, Model::Airbus_A380);
            original.setDistanceFromGroundControl(275);
            uint8_t buffer[PLANE_SERIALIZATION_SIZE];
            original.serialize(buffer);

            // Deserialize from buffer
            Plane deserialized(buffer);

            Assert::AreEqual(original.getId(), deserialized.getId());
            Assert::IsTrue(original.getCarrier() == deserialized.getCarrier());
            Assert::IsTrue(original.getModel() == deserialized.getModel());
            Assert::AreEqual(original.getDistanceFromGround(), deserialized.getDistanceFromGround());
            Assert::IsTrue(deserialized.getCountry() == Country::Country_Unknown); // Country not serialized
        }

        TEST_METHOD(Serialize_ProducesCorrectBuffer)
        {
            Plane plane(456, Carrier::WestJet, Country::Germany, Model::Airbus_A320neo);
            plane.setDistanceFromGroundControl(400);
            uint8_t buffer[PLANE_SERIALIZATION_SIZE];
            plane.serialize(buffer);

            // Verify buffer structure
            uint32_t id;
            memcpy(&id, buffer, sizeof(id));
            Assert::AreEqual(456u, id);

            Assert::IsTrue(static_cast<Carrier>(buffer[4]) == Carrier::WestJet);
            Assert::IsTrue(static_cast<Model>(buffer[5]) == Model::Airbus_A320neo);

            uint32_t distance;
            memcpy(&distance, buffer + 6, sizeof(distance));
            Assert::AreEqual(400u, distance);
        }

        TEST_METHOD(DecreaseDistance_ClampsAtZero)
        {
            Plane plane(789);
            plane.decreaseDistance(200);
            Assert::AreEqual(150u, plane.getDistanceFromGround());

            plane.decreaseDistance(250); // Try to go negative
            Assert::AreEqual(0u, plane.getDistanceFromGround());
        }

        TEST_METHOD(IncreaseDistance_UpdatesCorrectly)
        {
            Plane plane(999);
            plane.increaseDistance(100);
            Assert::AreEqual(450u, plane.getDistanceFromGround());
        }

        TEST_METHOD(TransactionNumber_IncrementsOnGetAndIncrease)
        {
            Plane plane(100);
            Assert::AreEqual(0u, plane.getCurrentTransactionNum());

            plane.getAndIncreaseTransactionNum();
            Assert::AreEqual(1u, plane.getCurrentTransactionNum());

            plane.getAndIncreaseTransactionNum();
            Assert::AreEqual(2u, plane.getCurrentTransactionNum());
        }

        TEST_METHOD(Setters_UpdateFields)
        {
            Plane plane(123);

            plane.setCarrier(Carrier::Frontier);
            Assert::IsTrue(plane.getCarrier() == Carrier::Frontier);

            plane.setCountry(Country::France);
            Assert::IsTrue(plane.getCountry() == Country::France);

            plane.setGoNoGo(Go_NoGo::NoGo);
            Assert::IsTrue(plane.getGoNoGo() == Go_NoGo::NoGo);
        }

        TEST_METHOD(GetFlagPath_ReturnsCorrectFormat)
        {
            // Test default constructor flag path
            Plane plane1(123);
            std::string expected1 = "../flags/" + plane1.getCountryName() + "_flag.png";
            Assert::AreEqual(expected1, plane1.getFlagPath());

            // Test parameterized constructor flag path
            Plane plane2(456, Carrier::Flair, Country::Andorra, Model::Boeing_747);
            std::string expected2 = "../flags/Andorra_flag.png"; // Matches constructor logic
            Assert::AreEqual(expected2, plane2.getFlagPath());
        }
    };

    TEST_CLASS(Client_Tests)
    {
    private:
        ClientStateMachine originalState;

        TEST_METHOD_INITIALIZE(Setup)
        {
            originalState = CSM;  // Save original state
            CSM = ClientStateMachine::Standby;  // Reset before each test
        }

        TEST_METHOD_CLEANUP(Cleanup)
        {
            CSM = originalState;  // Restore original state
        }

        // Helper function to create dummy plane
        Plane CreateTestPlane(uint32_t id = 123) {
            return Plane(id, Carrier::JetBlue, Country::Spain, Model::Cessna_Citation_X);
        }

    public:
        TEST_METHOD(StateTransition_ValidTransitions_Success)
        {
            Plane plane = CreateTestPlane();

            // Test valid transition sequence
            transitionCSM(ClientStateMachine::Ready, plane);
            Assert::IsTrue(CSM == ClientStateMachine::Ready);

            transitionCSM(ClientStateMachine::Initialization, plane);
            Assert::IsTrue(CSM == ClientStateMachine::Initialization);

            transitionCSM(ClientStateMachine::Connected, plane);
            Assert::IsTrue(CSM == ClientStateMachine::Connected);
        }

        TEST_METHOD(StateTransition_InvalidTransition_GoesToFailed)
        {
            Plane plane = CreateTestPlane();

            // Try invalid transition from Standby to Connected
            transitionCSM(ClientStateMachine::Connected, plane);
            Assert::IsTrue(CSM == ClientStateMachine::Failed);
            Assert::IsTrue(plane.getGoNoGo() == Go_NoGo::NoGo);
        }

        TEST_METHOD(RequestWeather_CreatesCorrectPacket)
        {
            Plane plane = CreateTestPlane();
            uint32_t initialTransactions = plane.getCurrentTransactionNum();

            requestWeather(plane);

            Assert::AreEqual(initialTransactions + 1u, plane.getCurrentTransactionNum());
        }

        TEST_METHOD(PlaneDistance_DecreasesWhenGoNoGoIsGo)
        {
            Plane plane = CreateTestPlane();
            plane.setGoNoGo(Go_NoGo::Go);
            uint32_t initialDistance = 200;
            plane.setDistanceFromGroundControl(initialDistance);

            plane.decreaseDistance(1);

            Assert::AreEqual(initialDistance - 1u, plane.getDistanceFromGround());
        }

        TEST_METHOD(PlaneDistance_IncreasesWhenGoNoGoIsNoGo)
        {
            Plane plane = CreateTestPlane();
            plane.setGoNoGo(Go_NoGo::NoGo);
            uint32_t initialDistance = 200;
            plane.setDistanceFromGroundControl(initialDistance);

            plane.increaseDistance(1);

            Assert::AreEqual(initialDistance + 1u, plane.getDistanceFromGround());
        }

        TEST_METHOD(PlaneDistance_NeverGoesBelowZero)
        {
            Plane plane = CreateTestPlane();
            plane.setGoNoGo(Go_NoGo::Go);
            plane.setDistanceFromGroundControl(0);

            plane.decreaseDistance(1);

            Assert::AreEqual(0u, plane.getDistanceFromGround());
        }

        TEST_METHOD(FlagTransmission_TriggersAt300km)
        {
            Plane plane = CreateTestPlane();
            bool sentFlag = false;

            // Simulate crossing threshold
            plane.setDistanceFromGroundControl(300);
            if (plane.getDistanceFromGround() <= 300 && !sentFlag) {
                sentFlag = true;
            }

            Assert::IsTrue(sentFlag);
        }

        TEST_METHOD(WeatherRequest_TriggersAt275km)
        {
            Plane plane = CreateTestPlane();
            bool requestedWeather = false;

            // Simulate crossing threshold
            plane.setDistanceFromGroundControl(275);
            if (plane.getDistanceFromGround() <= 275 && !requestedWeather) {
                requestedWeather = true;
            }

            Assert::IsTrue(requestedWeather);
        }

        TEST_METHOD(TransitionFromFailedToConnected_ResetsStateProperly)
        {
            Plane plane = CreateTestPlane();
            CSM = ClientStateMachine::Failed;

            transitionCSM(ClientStateMachine::Connected, plane);

            Assert::IsTrue(CSM == ClientStateMachine::Connected);
            Assert::IsTrue(plane.getGoNoGo() == Go_NoGo::Go);
        }

        TEST_METHOD(InvalidTransition_FromConnectedToInitialization_Fails)
        {
            Plane plane = CreateTestPlane();
            CSM = ClientStateMachine::Connected;

            transitionCSM(ClientStateMachine::Initialization, plane);

            Assert::IsTrue(CSM == ClientStateMachine::Failed);
        }
    };
}
