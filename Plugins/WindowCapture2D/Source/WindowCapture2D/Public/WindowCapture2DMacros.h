#pragma once

#define WC_LOG(Verbosity, Format, ...) UE_LOG(LogTemp, Verbosity, TEXT("[WC][%p] %s: " Format), this, TEXT(__FUNCTION__), ##__VA_ARGS__)