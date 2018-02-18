// Fill out your copyright notice in the Description page of Project Settings.

#include "LeaderboardManager.h"

#include "Tutorial.h"
#include "sha256.h"
#include <string>
#include <sstream>
#include "Json.h"

//NDK fix
template <typename T>
std::string to_string(T value)
{
	std::ostringstream os;
	os << value;
	return os.str();
}

ULeaderboardManager::ULeaderboardManager()
{
	//When the object is constructed, Get the HTTP module
	Http = &FHttpModule::Get();
	serverPHPpath = "http://yourHostingURL/Tutorial_LB/gameFunctions.php"; // replace with the gameFunctions URL
}

void ULeaderboardManager::OnResponseSendScore(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		//Create a pointer to hold the json serialized data
		TSharedPtr<FJsonObject> JsonObject;

		FString contenico = Response->GetContentAsString();
		//Create a reader pointer to read the json data
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

		//Deserialize the json data given Reader and the actual object to deserialize
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			//Get the value of the json object by field name
			int32 returnCode = JsonObject->GetIntegerField("returnCode");
			if (returnCode == 0)
			{
				FString receivedUserId = JsonObject->GetStringField("returnData");

				if (userId.IsEmpty())
					userId = receivedUserId;
				ShowSuccessSendScore();
			}
			else
			{
				//muestra error
				errorMessage = JsonObject->GetStringField("returnData");
				ShowFailedSend();
			}
		}
		else
		{
			errorMessage = "Error on data deserialization";
			ShowFailedSend();
		}
	}
	else
	{
		errorMessage = "Error on http request";
		ShowFailedSend();
	}
}

void ULeaderboardManager::OnResponseGetLeaderboard(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		//Create a pointer to hold the json serialized data
		TSharedPtr<FJsonObject> JsonObject;

		//Create a reader pointer to read the json data
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

		//Deserialize the json data given Reader and the actual object to deserialize
		if (FJsonSerializer::Deserialize(Reader, JsonObject))
		{
			int32 returnCode = JsonObject->GetIntegerField("returnCode");
			if (returnCode == 0)
			{
				//Get the value of the json object by field name
				TArray<TSharedPtr<FJsonValue>> arr = JsonObject->GetArrayField("returnData");

				t100_names.Empty();
				t100_score.Empty();

				for (int i = 0; i < arr.Num() - 1; ++i)
				{
					auto elemento = arr[i]->AsArray();

					t100_names.Add(elemento[1]->AsString());
					t100_score.Add(elemento[2]->AsNumber());
				}

				//personal result
				auto elemento = arr[arr.Num() - 1]->AsArray();
				if (elemento[0]->AsNumber() != 0)
				{
					personal_pos = elemento[0]->AsNumber();
					personal_name = elemento[1]->AsString();
					personal_score = elemento[2]->AsNumber();
				}
				else
				{
					personal_name = "";
					personal_score = 0;
				}

				//Output it to the engine
				ShowSuccessList();
			}
			else
			{
				//muestra error
				errorMessage = JsonObject->GetStringField("returnData");
				ShowFailedSend();
			}
		}
		else
		{
			errorMessage = "Error on data deserialization 2";
			ShowFailedSend();
		}
	}
	else
	{
		errorMessage = "Error on http request leaderboard";
		ShowFailedList();
	}
}

void ULeaderboardManager::setUserId(const FString& newuserId)
{
	userId = newuserId;
}

void ULeaderboardManager::sendScore(const FString& userName, const int32& score)
{
	FString data = "funcName=sendScore";
	std::string output1;
	std::string userName_str(TCHAR_TO_UTF8(*userName));
	if (!userId.IsEmpty())
	{
		data += "&userId=" + userId;
		std::string userid_str(TCHAR_TO_UTF8(*userId));
		output1 = sha256(userid_str + userName_str + "secret1" + to_string(score) + "secret2");
	}
	else
	{
		output1 = sha256(userName_str + "secret1" + to_string(score) + "secret2");
	}
	data += "&userName=" + userName + "&score=" + FString::FromInt(score) + "&userData=" + FString(output1.c_str());


	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ULeaderboardManager::OnResponseSendScore);
	//This is the url on which to process the request
	Request->SetURL(serverPHPpath);
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
	Request->SetContentAsString(data);
	Request->ProcessRequest();
}

void ULeaderboardManager::getLeaderboard()
{
	//Top100+ personalHS
	FString data = "funcName=getLeaderboard";

	if (!userId.IsEmpty())
		data += "&userId=" + userId;

	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &ULeaderboardManager::OnResponseGetLeaderboard);
	//This is the url on which to process the request
	Request->SetURL(serverPHPpath);
	Request->SetVerb("POST");
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	Request->SetHeader("Content-Type", "application/x-www-form-urlencoded");
	Request->SetContentAsString(data);
	Request->ProcessRequest();
}