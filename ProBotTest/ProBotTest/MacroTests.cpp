#include "stdafx.h"
#include "CppUnitTest.h"
#include "TechTree.h"
#include "BuildMinimax.h"
#include "TestUnit.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace ProBot;

namespace ProBotTest
{		
	
	TechTree test1 = TechTree{ Protoss_Arbiter, true, { TechTree{ Protoss_Gateway, false, {} }, TechTree{ Protoss_Observatory, false, {} } } };
	TechTree test2 = TechTree{ Protoss_Dark_Templar, true, { TechTree{ Protoss_Archon, false, {} }, TechTree{ Protoss_Zealot, false, {} } } };
	TechTree test3 = TechTree{ Protoss_Dark_Templar, true, { TechTree{ Protoss_Archon, true, {} }, TechTree{ Protoss_Zealot, false, {} } } };
	TechTree test4 = TechTree{ Protoss_Observer, true, {} };
	TechTree test5 = TechTree{ Protoss_Observatory, true, { TechTree{ Protoss_Zealot, false, {} }, TechTree{ Protoss_Observer, false, {} } } };
	TechTree test5a = TechTree{ Protoss_Observatory, true, { TechTree{ Protoss_Zealot, true, {} }, TechTree{ Protoss_Observer, false, {} } } };
	TechTree test5b = TechTree{ Protoss_Observatory, true, { TechTree{ Protoss_Zealot, false, {} }, TechTree{ Protoss_Observer, true, {} } } };
	TechTree test6 = TechTree{ Protoss_Nexus, true, { TechTree{ Protoss_Gateway, { TechTree{ Protoss_Zealot } } }, TechTree{ Protoss_Forge, { TechTree{ Protoss_Photon_Cannon } } } } };
	TechTree test7 = TechTree{ Protoss_Templar_Archives, true, { TechTree{ Protoss_Archon, true, {} }, TechTree{ Protoss_Dark_Templar, true, {} } } };
	TechTree test8 = TechTree{ Protoss_Dark_Templar};
	TechTree test9 = TechTree{ Protoss_Observatory, true, { TechTree{ Protoss_Observer } } };
	TechTree test10 = TechTree{ Protoss_Observatory, true, { TechTree{ Protoss_Observer, true, {} } } };
	TechTree test11 = TechTree{ Protoss_Nexus, true, { TechTree{ Protoss_Observatory, { TechTree{ Protoss_Observer } } }, TechTree{ Protoss_Probe } } };
	TechTree test12 = TechTree{ Protoss_Probe, false, {} };
	TechTree test13 = TechTree{ Protoss_Nexus, true, { TechTree{ Protoss_Gateway, false }, TechTree{ Protoss_Gateway, false, { TechTree{Protoss_Zealot} } } } };

	TEST_CLASS(TechTreeTest)
	{
	public:
		TEST_METHOD(TestParent){
			//Assert::AreEqual(Protoss_Nexus.getName(), BuildMinimax::getParentType(Protoss_Gateway).getName());
			Assert::AreEqual(Protoss_Observatory.getName(), getImmediateParent(Protoss_Observer, test11)->_unitType.getName());
		}
		TEST_METHOD(TestfindNextObtainableNodes){
			Assert::AreEqual(Protoss_Archon.getName(), findNextObtainableNodes(test2).at(0)._unitType.getName());
			Assert::AreEqual(Protoss_Zealot.getName(), findNextObtainableNodes(test2).at(1)._unitType.getName());
			Assert::AreEqual(2, static_cast<int>(findNextObtainableNodes(test2).size()));
		}
		TEST_METHOD(TestgetFirstDifference){
			Assert::AreEqual(Protoss_Archon.getName(), getFirstDifference(test3, test2).getName());
		}
		TEST_METHOD(TestfindLeafNodes){
			Assert::AreEqual(Protoss_Observer.getName(), findLeafNodes(test4).at(0)._unitType.getName());
			Assert::AreEqual(Protoss_Archon.getName(), findLeafNodes(test3).at(0)._unitType.getName());
			Assert::AreEqual(Protoss_Zealot.getName(), findLeafNodes(test3).at(1)._unitType.getName());
		}
		TEST_METHOD(TestfindNonBuildingLeafNodes){
			Assert::AreEqual(Protoss_Observer.getName(), findNonBuildingLeafNodes(test4).at(0)._unitType.getName());
			Assert::AreEqual(Protoss_Archon.getName(), findNonBuildingLeafNodes(test3).at(0)._unitType.getName());
		}
		TEST_METHOD(Testminimax){
			/*Assert::AreEqual(Protoss_Observer.getName(), BuildMinimax::minimax(3, BuildMinimax::SELF, test11, test8).first._subs.at(0)._subs.at(0)._unitType.getName());
			Assert::AreEqual(true, BuildMinimax::minimax(3, BuildMinimax::SELF, test11, test8).first._subs.at(0)._obtained);
			Assert::AreEqual(Protoss_Probe.getName(), BuildMinimax::minimax(3, BuildMinimax::SELF, test11, test8).first._subs.at(1)._unitType.getName());
			Assert::AreEqual(false, BuildMinimax::minimax(3, BuildMinimax::SELF, test11, test8).first._subs.at(1)._obtained);*/
		}


		TEST_METHOD(TestPrereq){
			//Assert::AreEqual(Protoss_Nexus.getName(), findPrereqNodes(Protoss_Probe, TERRAN_TREE).at(2).getName());
			//Assert::AreEqual(0, static_cast<int>(getUnitPrereqs(Protoss_Probe, TERRAN_TREE).size()));
			Assert::AreEqual(0, static_cast<int>(getUnitPrereqs(Protoss_Nexus, PROTOSS_TREE).size()));
			Assert::AreEqual(1, static_cast<int>(getUnitPrereqs(Protoss_Probe, PROTOSS_TREE).size()));
			Assert::AreEqual(Protoss_Gateway.getName(), getUnitPrereqs(Protoss_Zealot, PROTOSS_TREE).at(1).getName());
		}

		TEST_METHOD(TestTree2){
			Assert::AreEqual(Protoss_Zealot.getName(), findNextObtainableNodes(test2).at(1)._unitType.getName());
		}

		TEST_METHOD(TestMutable){
			Assert::AreEqual(false, test12._obtained);
			ProBot::setNodeObtained(Protoss_Probe, test12);
			Assert::AreEqual(true, test12._obtained);
		}

		TEST_METHOD(TestRemove){
			Assert::AreEqual(2, static_cast<int>(test1._subs.size()));
			removeNode(Protoss_Gateway, test1, false);
			Assert::AreEqual(1, static_cast<int>(test1._subs.size()));
		}

		TEST_METHOD(TestRemoveDuplicate){
			Assert::AreEqual(2, static_cast<int>(test13._subs.size()));
			setNonUniqueNode(&test13._subs.at(1), test13);
			Assert::AreEqual(1, static_cast<int>(test13._subs.size()));
			Assert::AreEqual(1, static_cast<int>(test13._subs.at(0)._subs.size()));
		}

		TEST_METHOD(TestObtained){
			Assert::AreEqual(Protoss_Observatory.getName(), getObtainedNodes(test5a).at(0).getName());
		}
	};
}