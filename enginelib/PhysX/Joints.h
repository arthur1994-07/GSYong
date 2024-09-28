//// ===============================================================================
////						  NVIDIA PHYSX SDK TRAINING PROGRAMS
////						 	   JOINT CREATION METHODS
////
////						    Written by Bob Schade, 5-1-06
//// ===============================================================================
//
//#ifndef JOINTS_H
//#define JOINTS_H
//
//#include "NxPhysics.h"
//
//NxFixedJoint* CreateFixedJoint(NxScene* gScene, physx::PxRigidActor* a0, physx::PxRigidActor* a1, const PxVec3& globalAnchor, const PxVec3& globalAxis);
//NxRevoluteJoint* CreateRevoluteJoint(NxScene* gScene, physx::PxRigidActor* a0, physx::PxRigidActor* a1, const PxVec3& globalAnchor, const PxVec3& globalAxis);
//NxSphericalJoint* CreateSphericalJoint(NxScene* gScene, physx::PxRigidActor* a0, physx::PxRigidActor* a1, const PxVec3& globalAnchor, const PxVec3& globalAxis, float fTwist, float fSwing);
//NxPrismaticJoint* CreatePrismaticJoint(NxScene* gScene, physx::PxRigidActor* a0, physx::PxRigidActor* a1, const PxVec3& globalAnchor, const PxVec3& globalAxis);
//NxCylindricalJoint* CreateCylindricalJoint(NxScene* gScene, physx::PxRigidActor* a0, physx::PxRigidActor* a1, const PxVec3& globalAnchor, const PxVec3& globalAxis);
//NxPointOnLineJoint* CreatePointOnLineJoint(NxScene* gScene, physx::PxRigidActor* a0, physx::PxRigidActor* a1, const PxVec3& globalAnchor, const PxVec3& globalAxis);
//NxPointInPlaneJoint* CreatePointInPlaneJoint(NxScene* gScene, physx::PxRigidActor* a0, physx::PxRigidActor* a1, const PxVec3& globalAnchor, const PxVec3& globalAxis);
//NxPulleyJoint* CreatePulleyJoint(NxScene* gScene, physx::PxRigidActor* a0, physx::PxRigidActor* a1, const PxVec3& pulley0, const PxVec3& pulley1, const PxVec3& globalAxis, NxReal distance, NxReal ratio, const NxMotorDesc& motorDesc);
//NxDistanceJoint* CreateDistanceJoint(NxScene* gScene, physx::PxRigidActor* a0, physx::PxRigidActor* a1, const PxVec3& anchor0, const PxVec3& anchor1, const PxVec3& globalAxis);
//
//NxSphericalJoint* CreateRopeSphericalJoint(NxScene* gScene, physx::PxRigidActor* a0, physx::PxRigidActor* a1, const PxVec3& globalAnchor, const PxVec3& globalAxis);
//NxSphericalJoint* CreateClothSphericalJoint(NxScene* gScene, physx::PxRigidActor* a0, physx::PxRigidActor* a1, const PxVec3& globalAnchor, const PxVec3& globalAxis);
//NxSphericalJoint* CreateBodySphericalJoint(NxScene* gScene, physx::PxRigidActor* a0, physx::PxRigidActor* a1, const PxVec3& globalAnchor, const PxVec3& globalAxis);
//NxRevoluteJoint* CreateWheelJoint(NxScene* gScene, physx::PxRigidActor* a0, physx::PxRigidActor* a1, const PxVec3& globalAnchor, const PxVec3& globalAxis);
//NxRevoluteJoint* CreateStepJoint(NxScene* gScene, physx::PxRigidActor* a0, physx::PxRigidActor* a1, const PxVec3& globalAnchor, const PxVec3& globalAxis);
//
//NxRevoluteJoint* CreateChassisJoint(NxScene* gScene, physx::PxRigidActor* a0, physx::PxRigidActor* a1, const PxVec3& globalAnchor, const PxVec3& globalAxis);
//NxFixedJoint* CreateCannonJoint(NxScene* gScene, physx::PxRigidActor* a0, physx::PxRigidActor* a1, const PxVec3& globalAnchor, const PxVec3& globalAxis);
//
//NxSphericalJoint* CreateBladeLink(NxScene* gScene, physx::PxRigidActor* a0, physx::PxRigidActor* a1, const PxVec3& globalAnchor, const PxVec3& globalAxis);
//
//NxRevoluteJoint* CreateRevoluteJoint2(NxScene* gScene, physx::PxRigidActor* a0, physx::PxRigidActor* a1,const PxVec3& localAnchor0,const PxVec3& localAnchor1,const PxVec3& localAxis0,const PxVec3& localAxis1);
//
//NxD6Joint* CreateD6Joint(NxScene* gScene, physx::PxRigidActor* a0, physx::PxRigidActor* a1, const PxVec3& globalAnchor, const PxVec3& globalAxis, float fTwist, float fSwingY, float fSwingZ, 
//						 float fRestitutionSwing, float fSpringSwing, float fDampingSwing, float fRestitutionTwist, float fSpringTwist, float fDampingTwist );
//
//#endif  // JOINTS_H
//
