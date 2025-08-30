from concurrent.futures import ThreadPoolExecutor
import zipfile
import os

def add_file_to_zip(zip_path, runtime_dir, file):
    abs_path = os.path.join(runtime_dir, file)
    with zipfile.ZipFile(zip_path, 'a', zipfile.ZIP_DEFLATED) as zipf:
        zipf.write(abs_path, file)

def create_versioned_archive(runtime_dir, project_name, program_version):
    zip_name = f"{project_name}_{program_version}.zip"
    zip_path = os.path.join(runtime_dir, "..", zip_name)

    files = []
    for root, dirs, filenames in os.walk(runtime_dir):
        for f in filenames:
            rel_path = os.path.relpath(os.path.join(root, f), runtime_dir)
            files.append(rel_path)

    # Многопоточная запись (каждому потоку своя очередь)
    with ThreadPoolExecutor(max_workers=os.cpu_count()) as executor:
        executor.map(lambda f: add_file_to_zip(zip_path, runtime_dir, f), files)

    return zip_path
