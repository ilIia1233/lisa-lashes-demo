#include <zippuccino/zipper.h>

int main(int argc, char **argv) {
  zippuccino::Zipper zipper;
  zipper.add("../example");
  zipper.add("../include");
  zipper.add("../lib");
  zipper.add("../src");
  zipper.add("../CMakeLists.txt");
  zipper.add("../README.md");
  zipper.add("../LICENSE");
  zipper.add("../.gitignore");
  zipper.add("../.gitmodules");
  zipper.zip();
  const std::string zipPath = "../zippuccino.zip";

  std::ofstream zipFile(zipPath, std::ios::binary);
  if (!zipFile.is_open()) {
    std::cerr << "Unable to open output file" << std::endl;
    return EXIT_FAILURE;
  }

  logger::success("Creating zip file " + zipPath);
  logger::success("Estimated size: " + std::to_string(zipper.getSize()) +
                  " bytes");
  while (!zipper.isFinished()) {
    const std::string header = zipper.getHeader();
    zipFile.write(header.c_str(), header.size());

    std::string path = zipper.getCurrentFile();
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
      std::cerr << "Unable to open file: " << path << std::endl;
      return EXIT_FAILURE;
    }

    std::copy(std::istreambuf_iterator<char>(file),
              std::istreambuf_iterator<char>(),
              std::ostreambuf_iterator<char>(zipFile));
    file.close();
    logger::info("Added file: " + path);
  }

  const std::string footer = zipper.getFooter();
  zipFile.write(footer.c_str(), footer.size());
  zipFile.close();
  logger::success("Zip file created successfully");
  logger::success("Final zip size: " +
                  std::to_string(brewtils::os::file::size(zipPath)) + " bytes");

  return EXIT_SUCCESS;
}