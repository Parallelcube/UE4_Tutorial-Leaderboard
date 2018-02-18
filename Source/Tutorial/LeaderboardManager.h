#pragma once

#include "Object.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "LeaderboardManager.generated.h"

/**
*
*/
UCLASS(Blueprintable, BlueprintType)
class TUTORIAL_API ULeaderboardManager : public UObject
{
	GENERATED_BODY()

private:
	FHttpModule* Http;
	FString	serverPHPpath;
	void OnResponseSendScore(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnResponseGetLeaderboard(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

public:

	//personal userId
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Variable")
		FString userId;

	//Leaderboard data Top 100
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Variable")
		TArray<FString> t100_names;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Variable")
		TArray<int32> t100_score;

	//Leaderboard personal data
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Variable")
		int32 personal_pos;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Variable")
		FString personal_name;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Variable")
		int32 personal_score;

	// returned error message
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Variable")
		FString errorMessage;

	// client functions
	UFUNCTION(BlueprintCallable, Category = "LBM_funtions")
		void setUserId(const FString& newuserId);
	UFUNCTION(BlueprintCallable, Category = "LBM_funtions")
		void sendScore(const FString& userName, const int32& score);
	UFUNCTION(BlueprintCallable, Category = "LBM_funtions")
		void getLeaderboard();

	//Callbacks
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Function")
		void ShowSuccessSendScore();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Function")
		void ShowFailedSend();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Function")
		void ShowSuccessList();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Function")
		void ShowFailedList();

	// Sets default values for this actor's properties
	ULeaderboardManager();
};
