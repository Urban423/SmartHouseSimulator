import subprocess
import sys
from pathlib import Path
from versionManager import create_versioned_archive

RUNTIME_DIR = "/workspace/build/OnlineAlpha"
PROJECT_NAME = "OnlineAlpha"
PROGRAM_VERSION = "1.0"

PLATFORM = sys.argv[1] if len(sys.argv) > 1 else "windows"

PROJECT_DIR = Path("/workspace")
CACHE_FILE = PROJECT_DIR / ".CMakeFiles" / "CMakeCache.txt"
CMAKELISTS_FILE = PROJECT_DIR / "CMakeLists.txt"
CMAKEPRESETS_FILE = PROJECT_DIR / "CMakePresets.json"
CMAKE_DIR = PROJECT_DIR / "cmake"

def need_configure() -> bool:
    if not CACHE_FILE.exists():
        return True
    if CMAKELISTS_FILE.stat().st_mtime > CACHE_FILE.stat().st_mtime:
        return True
    if CMAKEPRESETS_FILE.stat().st_mtime > CACHE_FILE.stat().st_mtime:
        return True
    return False

def main():
    if need_configure():
        print("Running CMake configuration...")
        subprocess.check_call(["cmake", "--preset", PLATFORM], cwd=PROJECT_DIR)
    else:
        print("CMake configuration up to date.")

    print("Building project...")
    subprocess.check_call(["cmake", "--build", ".CMakeFiles"], cwd=PROJECT_DIR)

    print("Updating version archive...")
    #create_versioned_archive(RUNTIME_DIR, PROJECT_NAME, PROGRAM_VERSION)

if __name__ == "__main__":
    main()
