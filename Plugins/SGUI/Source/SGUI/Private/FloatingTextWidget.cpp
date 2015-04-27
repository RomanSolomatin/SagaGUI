#include "SGUI.h"
#include "FloatingTextWidget.h"

UFloatingTextWidget* UFloatingTextWidget::Create(APlayerController* masterController, FString message, FLinearColor textColor)
{
	auto widget = InstantiateWidget<UFloatingTextWidget>(masterController);
	widget->messageLabel = Cast<UTextBlock>(widget->GetWidgetFromName(TEXT("MessageLabel")));
	widget->messageLabel->SetText(FText::FromString(message));
	widget->SetColor(textColor);

	return widget;
}

void UFloatingTextWidget::Spawn(APlayerController* masterController, FString message, FLinearColor textColor /*= FLinearColor::White*/)
{
	if (!masterController) return;

	auto widget = Create(masterController, message, textColor);

	GetSlot(widget->messageLabel)->SetPosition(GetViewportCenter());
}

void UFloatingTextWidget::SpawnAtPosition(APlayerController* masterController, FVector2D screenPosition, FString message, FLinearColor textColor /*= FLinearColor::White*/)
{
	if (!masterController) return;

	auto widget = Create(masterController, message, textColor);

	GetSlot(widget->messageLabel)->SetPosition(screenPosition / GetViewportScale());
}

void UFloatingTextWidget::SpawnAtActor(APlayerController* masterController, AActor* targetActor, FString message, FVector offset /*= FVector::ZeroVector*/, FLinearColor textColor /*= FLinearColor::White*/)
{
	if (!masterController) return;

	auto widget = Create(masterController, message, textColor);

	FVector2D screenPos;
	masterController->ProjectWorldLocationToScreen(targetActor->GetActorLocation() + offset, screenPos);
	GetSlot(widget->messageLabel)->SetPosition(screenPos);
}

void UFloatingTextWidget::Tick_Implementation(FGeometry myGeometry, float inDeltaTime)
{
	auto curPos = GetSlot(messageLabel)->GetPosition();

	GetSlot(messageLabel)->SetPosition(curPos + FVector2D(0, -FloatingSpeed * inDeltaTime));

	auto opacity = FMath::InterpExpoOut(messageLabel->ColorAndOpacity.GetSpecifiedColor().A,
		FMath::Clamp(curPos.Y / (GetViewportSize().Y / 2), 0.f, 1.f),
		inDeltaTime * FadeSpeed);
	auto color = messageLabel->ColorAndOpacity.GetSpecifiedColor();
	color.A = opacity;
	SetColor(color);

	if (curPos.Y < 0) RemoveFromViewport();
}

void UFloatingTextWidget::SetColor(const FLinearColor& color)
{
	messageLabel->SetColorAndOpacity(color);
	auto shadowColor = messageLabel->ShadowColorAndOpacity;
	// base shadow opacity is .8 (.2 less than base text opacity), so excluding it
	messageLabel->SetShadowColorAndOpacity(FLinearColor(shadowColor.R, shadowColor.G, shadowColor.B, FMath::Clamp(color.A - .2f, 0.f, 1.f)));
}


