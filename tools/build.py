import subprocess
from pathlib import Path
import sys
import shutil

ROOT_DIR = Path(__file__).parent.parent.resolve()
PLATFORM = sys.argv[1] if len(sys.argv) > 1 else "windows"
DOCKER_BUILD_DIR = Path("/workspace")
BUILD_DIR = ROOT_DIR / "build"

def sync_sources():
    """Синхронизация проекта в volume с помощью rsync"""
    # Опции:
    # -a — архивный режим (рекурсивно + сохраняет права, временные метки)
    # --delete — удаляет лишние файлы в целевой папке
    # --exclude 'build' — исключаем папку build
    subprocess.run([
        "rsync",
        "-a",
        "-u",
        "--delete",
        "--exclude", "build",
        "--exclude", ".git",
        "--exclude", ".CMakeFiles",
        f"{ROOT_DIR}/",
        f"{DOCKER_BUILD_DIR}/"
    ], check=True)

def build_inside_docker():
    # Конфигурация
    subprocess.run(["cmake", "--preset", PLATFORM], cwd=DOCKER_BUILD_DIR, check=True)
    # Сборка
    subprocess.run(["cmake", "--build", "./.CMakeFiles"], cwd=DOCKER_BUILD_DIR, check=True)

def sync_back():
    docker_build = DOCKER_BUILD_DIR / "build"
    if docker_build.exists():
        subprocess.run([
            "rsync",
            "-a",
            "--delete",
            f"{docker_build}/",
            f"{BUILD_DIR}/"
        ], check=True)

def main():
    print(f"Building for {PLATFORM}...")
    print("Build finished.")

if __name__ == "__main__":
    main()