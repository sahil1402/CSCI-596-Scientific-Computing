#pragma once
#include <filesystem>
namespace harbortune {
struct VerificationSummary { int passed=0; int failed=0; };
VerificationSummary run_verification(const std::filesystem::path& output_directory);
}
