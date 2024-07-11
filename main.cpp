#include <archive.h>
#include <CommandsParser.h>
#include <Hamming.h>

int main(int argc, char** argv) {
    CommandsParser parser{argc, argv};
    char command = parser.getCommand();

    Archive archive;
    std::string archname = parser.archive;
    int word_size = parser.word_size;

    if (command == 'c') {
        archive.newFile(archname + ".decoded");
        archive.create(parser.free_args);
        Encoder encoder{archname + ".decoded", archname};
        encoder.encode(word_size);
    } else if (command == 'l') {
        Decoder decoder{archname, archname + ".decoded"};
        decoder.decode(word_size);
        archive.read(archname + ".decoded");
        for (int i = 0; i < archive.files.size(); i++) {
            std::cout << archive.files[i].name << std::endl;
        }
        Encoder encoder{archname + ".decoded", archname};
        encoder.encode(word_size);
    } else if (command == 'x') {
        Decoder decoder{archname, archname + ".decoded"};
        archive.read(archname + ".decoded");
        if (parser.free_args.size() == 0) {
            archive.extract();
        } else {
            archive.extract(parser.free_args);
        }
        Encoder encoder{archname + ".decoded", archname};
        encoder.encode(word_size);
    } else if (command == 'a') {
        Decoder decoder{archname, archname + ".decoded"};
        decoder.decode(word_size);
        archive.read(archname + ".decoded");
        archive.append(parser.free_args[0]);
        Encoder encoder{archname + ".decoded", archname};
        encoder.encode(word_size);
    } else if (command == 'd') {
        Decoder decoder{archname, archname + ".decoded"};
        decoder.decode(word_size);
        archive.read(archname + ".decoded");
        archive.deleteFile(parser.free_args[0]);
        Encoder encoder{archname + ".decoded", archname};
        encoder.encode(word_size);
    } else if (command == 'A') {
        archive.newFile(archname + ".decoded");

        std::string arch1 = parser.free_args[0];
        std::string arch2 = parser.free_args[1];
        Decoder decoder1{arch1, arch1 + ".decoded"};
        Decoder decoder2{arch2, arch2 + ".decoded"};
        decoder1.decode(word_size);
        decoder2.decode(word_size);

        archive.concatenate(arch1 + ".decoded", arch2 + ".decoded");

        arch1 = arch1 + ".decoded";
        arch2 = arch2 + ".decoded";
        std::string arch_merged = archname + ".decoded";
        Encoder encoder1{arch1, arch1 + ".decoded"};
        Encoder encoder2{arch2, arch2 + ".decoded"};
        Encoder encoder_merged{arch_merged, arch_merged + ".decoded"};
        encoder1.encode(word_size);
        encoder2.encode(word_size);
        encoder_merged.encode(word_size);
    }
}
