#include "KernelGameplayTags.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_AI_DetectTarget,      "GameplayEvent.AI.DetectTarget");

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_Damage,             "SetByCaller.Damage");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_DamageImmunity,     "Status.Immunity.Damage");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_DamageSelfDestruct, "Gameplay.DamageSelfDestruct");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_FellOutOfWorld,     "Gameplay.FellOutOfWorld");
UE_DEFINE_GAMEPLAY_TAG(TAG_Kernel_Damage_Message,       "Kernel.Damage.Message");

UE_DEFINE_GAMEPLAY_TAG(TAG_GameplayEvent_Melee_Hit,     "GameplayEvent.Melee.Hit");

UE_DEFINE_GAMEPLAY_TAG(TAG_Kernel_EnemyState_Changed,   "Kernel.AI.Message.TargetChanged");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_WeaponSwap,            "InputTag.Weapon.Swap");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_MouseLook,             "InputTag.Look.Mouse");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_Move,                  "InputTag.Move");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_Sprint,                "InputTag.Sprint");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_Blocked,               "Status.Block.Input");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_Inventory,             "InputTag.Inventory");
UE_DEFINE_GAMEPLAY_TAG(TAG_Input_ScoreBoard,            "InputTag.ScoreBoard");

UE_DEFINE_GAMEPLAY_TAG(TAG_Interact_PlayerReady,        "Status.Player.Ready");
UE_DEFINE_GAMEPLAY_TAG(TAG_Interact_OnFocus,            "Interact.OnFocus");
UE_DEFINE_GAMEPLAY_TAG(TAG_Interact_EndFocus,           "Interact.EndFocus");
UE_DEFINE_GAMEPLAY_TAG(TAG_Interact_Crate,              "Interact.Crate");

UE_DEFINE_GAMEPLAY_TAG(TAG_QuickBar_Message_SlotsChanged,       "Kernel.QuickBar.Message.SlotsChanged");
UE_DEFINE_GAMEPLAY_TAG(TAG_QuickBar_Message_ActiveIndexChanged, "Kernel.QuickBar.Message.ActiveIndexChanged");
UE_DEFINE_GAMEPLAY_TAG(TAG_Stats_DamageTotal_Changed,           "Kernel.Damage.Total.Changed");

UE_DEFINE_GAMEPLAY_TAG(TAG_Status_Death_Dying,          "Status.Death.Dying")
UE_DEFINE_GAMEPLAY_TAG(TAG_Status_Death_Dead,           "Status.Death.Dead")
UE_DEFINE_GAMEPLAY_TAG(TAG_Status_Sprint,               "Status.Sprint")

UE_DEFINE_GAMEPLAY_TAG(TAG_UI_Warning,                  "UI.Warning")
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_ToggleLayout,             "UI.Action.ToggleLayout")
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_ToastMessage,             "UI.Action.ToastMessage")
UE_DEFINE_GAMEPLAY_TAG(TAG_UI_RollAffix,                "UI.Action.RollAffix")