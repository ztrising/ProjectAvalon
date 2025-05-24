/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "Widget_PlayerStatus.h"

#include "../Gameplay/Actor/AvalonActor.h"
#include "../Gameplay/Components/StatsComponent.h"
#include "../Gameplay/AvalonGameState.h"

unsigned short GetColorForStat(std::string Stat)
{
	unsigned short Color = (AVALON_FG_MAGENTA);

	if (Stat == "Health")
	{
		Color = (AVALON_FG_RED | AVALON_FG_INTENSITY);
	}
	else if (Stat == "Stamina")
	{
		Color = (AVALON_FG_GREEN);
	}
	else if (Stat == "Hunger")
	{
		Color = (AVALON_FG_YELLOW);
	}
	else if (Stat == "Thirst")
	{
		Color = (AVALON_FG_CYAN | AVALON_FG_INTENSITY);
	}

	return Color;
}

Widget_PlayerStatus::Widget_PlayerStatus()
{
	mStartVisible = false;
}

void Widget_PlayerStatus::Construct(const char* WidgetAsset)
{
	AvalonWidget::Construct("W_PlayerStatus_Frame.xml");
}

void Widget_PlayerStatus::OnGameLoaded()
{
	AvalonWidget::OnGameLoaded();
	Show();
}

void Widget_PlayerStatus::Show()
{
	AvalonWidget::Show();

	AvalonActor* Player = AvalonGameState::GetPlayerActor();
	StatsComponent* StatsComp = Player->GetComponent<StatsComponent>();
	

	FCoord Spacing = FCoord(5, 0);
	short XOffset = 25;

	std::vector<std::string> DisplayOrder = { "Health", "Stamina", "Hunger", "Thirst", "Toxicity" };
	for (int i = 0; i < DisplayOrder.size(); ++i)
	{
		HardUnitRef WidgetRef = AddChild<AvalonWidget>("W_PlayerStatus_StatTextArea.xml");
		std::string StatID = DisplayOrder[i];
		mStatDisplays[StatID] = WidgetRef;

		AvalonWidget* StatWidget = Get<AvalonWidget>(WidgetRef);
		StatWidget->Show();
		StatWidget->SetPosition(Spacing);
		Spacing.X += XOffset;

		unsigned short Color = GetColorForStat(StatID);
		StatWidget->ApplyAttributes(Color);

		std::string DisplayText = ">> " + StatID + " >>";
		StatWidget->SetText(DisplayText.c_str(), FCoord(), (AVALON_FG_WHITE), true);

		FAvalonStat* Stat = Get<FAvalonStat>(StatsComp->mStats[StatID]);
		HandleStatValueChange(StatID, Stat->mCurrentValue
									, Stat->mMaxValue.GetValue());
	}

	StatsComponent::StatsEvent::Callback Callback = Widget_PlayerStatus::HandleStatValueChange;
	StatsComp->mOnStatChanged.BindEvent(this, Callback);
}

void Widget_PlayerStatus::HandleStatValueChange(std::string Stat, float Value, float MaxValue)
{
	// The element accepts up to 40 units, divided over two rows.
	// "Max" is displayed as a putting the '[' or ']' on the screen
	// "Current" is displayed as coloring in
	
	float NumDisplayableUnits = 40;
	float MaxDisplayableValue = 200;
	if (MaxValue < 1) { MaxValue = 1; }
	else if (MaxValue > MaxDisplayableValue) { MaxValue = MaxDisplayableValue; }

	float MaxPct = MaxValue / MaxDisplayableValue;
	unsigned int NumSections = (MaxPct * NumDisplayableUnits);

	if (Value < 1) { Value = 1; }
	else if (Value > MaxValue) { Value = MaxValue; }

	float ValuePct = Value / MaxDisplayableValue;
	unsigned int NumFilledSections = (ValuePct * NumDisplayableUnits);

	AvalonWidget* StatWidget = Get<AvalonWidget>(mStatDisplays[Stat]);

	for (unsigned int PosY = 1; PosY < 3; ++PosY)
	{
		for (unsigned int PosX = 0; PosX < 20; ++PosX)
		{
			FCoord TextPos = FCoord(PosX, PosY);
			int Index = ((PosY-1) * 20) + PosX;

			if (Index < NumSections)
			{
				unsigned short Color = (Index < NumFilledSections) ? GetColorForStat(Stat) : (AVALON_FG_WHITE);
				const char* Char = (PosX == 0 || (PosX % 2) == 0) ? "[" : "]";
				StatWidget->SetText(Char, TextPos, Color, true);
			}
			else
			{
				StatWidget->SetText(" ", TextPos, 0, true);
			}
		}
	}

}

/*static*/ void Widget_PlayerStatus::HandleStatValueChange(IAvalonUnit* Listener, std::string StatID)
{
	Widget_PlayerStatus* Widget = static_cast<Widget_PlayerStatus*>(Listener);

	AvalonActor* Player = AvalonGameState::GetPlayerActor();
	StatsComponent* StatsComp = Player->GetComponent<StatsComponent>();
	FAvalonStat* Stat = Get<FAvalonStat>(StatsComp->mStats[StatID]);
	Widget->HandleStatValueChange(StatID, Stat->mCurrentValue
										, Stat->mMaxValue.GetValue());
}
