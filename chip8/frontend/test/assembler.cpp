#include <gtest/gtest.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <vector>

// The amount of memory that is allocated at the moment can be measured with the address sanitizer or with the glibc
#if defined(__SANITIZE_ADDRESS__)
#define CP8_ADDRESS_SANITIZER 1
#elif defined(__has_feature)
#if __has_feature(address_sanitizer)
#define CP8_ADDRESS_SANITIZER 1
#endif
#endif
#if defined(CP8_ADDRESS_SANITIZER)
// Part of the interface of the sanitizer runtime (gcc does not ship the header sanitizer/allocator_interface.h)
extern "C" size_t __sanitizer_get_current_allocated_bytes();
#define CP8_ALLOCATED_BYTES() (__sanitizer_get_current_allocated_bytes())
#elif defined(__GLIBC__) && defined(__GLIBC_PREREQ)
#if __GLIBC_PREREQ(2, 33)
#include <malloc.h>
static size_t CurrentlyAllocatedBytes() {
    struct mallinfo2 const info = mallinfo2();
    return info.uordblks + info.hblkhd;
}
#define CP8_ALLOCATED_BYTES() (CurrentlyAllocatedBytes())
#endif
#endif

extern "C" {
#include "base/exit_code.h"
#include "frontend/assembler.h"
}

namespace {

constexpr uint16_t kProgramStart = 0x200;

/// Assembles the source code into the memory (the assembler takes ownership of the copy of the source)
void Assemble(std::string const & source, uint8_t * memory) {
    char * copy = (char *)malloc(source.size() + 1);
    ASSERT_NE(nullptr, copy);
    memcpy(copy, source.c_str(), source.size() + 1);
    assembler_t assembler;
    ASSERT_EQ(0, assembler_initialize(&assembler, copy));
    ASSERT_EQ(0, assembler_process_file(&assembler, memory));
}

/// Assembles the text section
std::string Program(std::string const & instructions) {
    return "section .text:\n" + instructions + "\nsection .data:\n";
}

uint16_t Opcode(uint8_t const * memory, uint16_t index) {
    return (memory[kProgramStart + index * 2] << 8) | memory[kProgramStart + index * 2 + 1];
}

class AssemblerTest : public ::testing::Test {
  protected:
    void SetUp() override {
        memset(memory, 0, sizeof(memory));
    }

    uint8_t memory[4096];
};

struct MnemonicCase {
    char const * source;
    uint16_t opcode;
};

class MnemonicTest : public AssemblerTest, public ::testing::WithParamInterface<MnemonicCase> {};

TEST_P(MnemonicTest, AssemblesToOpcode) {
    Assemble(Program(std::string("    ") + GetParam().source + "\n    NOP"), memory);
    EXPECT_EQ(GetParam().opcode, Opcode(memory, 0)) << GetParam().source;
    EXPECT_EQ(0x0001, Opcode(memory, 1));
}

INSTANTIATE_TEST_SUITE_P(
    Mnemonics, MnemonicTest,
    ::testing::Values(
        MnemonicCase{"MOV V1 V0", 0x8100}, MnemonicCase{"MOV VA VB", 0x8AB0}, MnemonicCase{"MOV vf v1", 0x8F10},
        MnemonicCase{"MOV V0 0x1F", 0x601F}, MnemonicCase{"MOV I 0x900", 0xA900}, MnemonicCase{"MOV DT V3", 0xF315},
        MnemonicCase{"MOV ST V3", 0xF318}, MnemonicCase{"MOV V3 DT", 0xF307}, MnemonicCase{"ADD V0 V1", 0x8014},
        MnemonicCase{"ADD V0 0x07", 0x7007}, MnemonicCase{"ADD I V2", 0xF21E}, MnemonicCase{"SUB V0 V1", 0x8015},
        MnemonicCase{"MOVO V0 V1", 0x8011}, MnemonicCase{"MOVA V0 V1", 0x8012}, MnemonicCase{"MOVX V0 V1", 0x8013},
        MnemonicCase{"MOVS V0 V1", 0x8017}, MnemonicCase{"SKE V0 V1", 0x5010}, MnemonicCase{"SKE V0 0x3F", 0x303F},
        MnemonicCase{"SKNE V0 V1", 0x9010}, MnemonicCase{"SKNE V0 0x3F", 0x403F}, MnemonicCase{"SKP V3", 0xE39E},
        MnemonicCase{"SKNP V3", 0xE3A1}, MnemonicCase{"DSP V0 V1 0x05", 0xD015}, MnemonicCase{"RND V2 0x07", 0xC207},
        MnemonicCase{"STK V4", 0xF40A}, MnemonicCase{"STBC V5", 0xF533}, MnemonicCase{"STMR V3", 0xF355},
        MnemonicCase{"STMR VF", 0xFF55}, MnemonicCase{"FMR V3", 0xF365}, MnemonicCase{"FMR V0", 0xF065},
        MnemonicCase{"TGS", 0x00E1}, MnemonicCase{"CLS", 0x00E0}, MnemonicCase{"RET", 0x00EE},
        MnemonicCase{"EXT", 0x0002}, MnemonicCase{"JMP 0x234", 0x1234}, MnemonicCase{"CAL 0x300", 0x2300}),
    [](::testing::TestParamInfo<MnemonicCase> const & info) { return "Case" + std::to_string(info.index); });

TEST_F(AssemblerTest, CommentsAndEmptyLinesAreIgnored) {
    Assemble(Program("# comment\n\n    MOV V0 0x01   # trailing comment\n\n    MOV V1 V0\n"), memory);
    EXPECT_EQ(0x6001, Opcode(memory, 0));
    EXPECT_EQ(0x8100, Opcode(memory, 1));
}

TEST_F(AssemblerTest, AddAcceptsAllKindsOfWhitespaceBetweenTheOperands) {
    // ADD has to look at the second operand to tell "ADD VX VY" from "ADD VX 0xNN"
    Assemble(Program("    ADD V1\tV2\n"
                     "    ADD V1\t0x05\n"
                     "    ADD V1 \t  V2\n"
                     "    ADD\tV1\t \t0x05\n"
                     "    ADD V1\r\n    V2\n"
                     "    ADD V1 # the second operand follows\n    0x05\n"
                     "    ADD V1 V2\n"
                     "    ADD V1 0x05\n"
                     "    ADD V1\n"
                     "\n"
                     "    0x05"),
             memory);
    uint16_t const expected[] = {0x8124, 0x7105, 0x8124, 0x7105, 0x8124, 0x7105, 0x8124, 0x7105, 0x7105};
    for (size_t i = 0; i < sizeof(expected) / sizeof(expected[0]); i++) {
        EXPECT_EQ(expected[i], Opcode(memory, i)) << i;
    }
}

TEST_F(AssemblerTest, AllMemoryIsFreedAfterAssembling) {
#if defined(CP8_ALLOCATED_BYTES)
    // Many labels, so that the tables of the labels and of the references to labels have to grow and have empty slots
    std::string instructions = "_start:\n";
    for (int i = 0; i < 100; i++) {
        instructions += "    JMP forward" + std::to_string(i) + "\n";
    }
    for (int i = 0; i < 100; i++) {
        instructions += "_forward" + std::to_string(i) + ":\n    JMP start\n";
    }
    std::string const source = Program(instructions);
    // The first run allocates what is only allocated once (buffers of the standard library, ...)
    Assemble(source, memory);
    size_t const allocatedBefore = CP8_ALLOCATED_BYTES();
    Assemble(source, memory);
    EXPECT_EQ(allocatedBefore, CP8_ALLOCATED_BYTES());
#else
    GTEST_SKIP() << "The allocated memory can not be measured with this compiler and standard library";
#endif
}

TEST_F(AssemblerTest, DataSectionIsStoredAtTheOrigin) {
    Assemble("section .text:\n    NOP\nsection .data:\n    org 0x900\n    0xfc\n    0x80\n", memory);
    EXPECT_EQ(0xfc, memory[0x900]);
    EXPECT_EQ(0x80, memory[0x901]);
}

TEST_F(AssemblerTest, BackwardJump) {
    Assemble(Program("_start:\n    NOP\n    JMP start"), memory);
    EXPECT_EQ(0x1200, Opcode(memory, 1));
}

TEST_F(AssemblerTest, ForwardJump) {
    Assemble(Program("    JMP end\n    NOP\n_end:\n    NOP"), memory);
    EXPECT_EQ(0x1204, Opcode(memory, 0));
}

TEST_F(AssemblerTest, ForwardAndBackwardCall) {
    Assemble(Program("    CAL later\n    EXT\n_earlier:\n    RET\n_later:\n    CAL earlier\n    RET"), memory);
    EXPECT_EQ(0x2206, Opcode(memory, 0));
    EXPECT_EQ(0x2204, Opcode(memory, 3));
}

TEST_F(AssemblerTest, SeveralReferencesToTheSameForwardLabel) {
    Assemble(Program("    JMP end\n    JMP end\n    CAL end\n    NOP\n_end:\n    NOP\n    JMP end"), memory);
    EXPECT_EQ(0x1208, Opcode(memory, 0));
    EXPECT_EQ(0x1208, Opcode(memory, 1));
    EXPECT_EQ(0x2208, Opcode(memory, 2));
    EXPECT_EQ(0x1208, Opcode(memory, 5));
}

TEST_F(AssemblerTest, LabelsMayBeShortAndContainDigitsAndUnderscores) {
    Assemble(Program("_a:\n    NOP\n_ab:\n    NOP\n_loop2:\n    NOP\n_my_label_3:\n    NOP\n"
                     "    JMP a\n    JMP ab\n    JMP loop2\n    JMP my_label_3"),
             memory);
    EXPECT_EQ(0x1200, Opcode(memory, 4));
    EXPECT_EQ(0x1202, Opcode(memory, 5));
    EXPECT_EQ(0x1204, Opcode(memory, 6));
    EXPECT_EQ(0x1206, Opcode(memory, 7));
}

TEST_F(AssemblerTest, LabelsThatAreNotTheSamePrefixAreNotMixedUp) {
    Assemble(Program("    JMP abc\n    JMP ab\n_ab:\n    NOP\n_abc:\n    NOP"), memory);
    EXPECT_EQ(0x1206, Opcode(memory, 0));
    EXPECT_EQ(0x1204, Opcode(memory, 1));
}

TEST_F(AssemblerTest, LabelAbove0x2FF) {
    std::string instructions = "    JMP far\n";
    for (int i = 0; i < 200; i++) {
        instructions += "    NOP\n";
    }
    instructions += "_far:\n    NOP\n    JMP far";
    Assemble(Program(instructions), memory);
    // 0x200 + 201 * 2 = 0x392
    EXPECT_EQ(0x1392, Opcode(memory, 0));
    EXPECT_EQ(0x1392, Opcode(memory, 202));
}

TEST_F(AssemblerTest, ManyDifferentLabels) {
    std::string instructions;
    for (int i = 0; i < 40; i++) {
        instructions += "    JMP target" + std::to_string(i) + "\n";
    }
    for (int i = 0; i < 40; i++) {
        instructions += "_target" + std::to_string(i) + ":\n    NOP\n";
    }
    Assemble(Program(instructions), memory);
    for (int i = 0; i < 40; i++) {
        EXPECT_EQ(0x1000 | (kProgramStart + 80 + i * 2), Opcode(memory, i)) << i;
    }
}

using AssemblerDeathTest = AssemblerTest;

TEST_F(AssemblerDeathTest, UndefinedLabelIsAnError) {
    EXPECT_EXIT(Assemble(Program("    JMP nowhere"), memory), ::testing::ExitedWithCode(EXIT_CODE_ASSEMBLER_ERROR), "");
}

TEST_F(AssemblerDeathTest, DuplicateLabelIsAnError) {
    EXPECT_EXIT(Assemble(Program("_a:\n    NOP\n_a:\n    NOP"), memory),
                ::testing::ExitedWithCode(EXIT_CODE_ASSEMBLER_ERROR), "defined more than once");
}

TEST_F(AssemblerDeathTest, LabelMustStartWithALetter) {
    EXPECT_EXIT(Assemble(Program("_1a:\n    NOP"), memory), ::testing::ExitedWithCode(EXIT_CODE_ASSEMBLER_ERROR),
                "must start with a letter");
}

TEST_F(AssemblerDeathTest, LabelMustEndWithAColon) {
    EXPECT_EXIT(Assemble(Program("_a\n    NOP"), memory), ::testing::ExitedWithCode(EXIT_CODE_ASSEMBLER_ERROR), "");
}

TEST_F(AssemblerDeathTest, StoreMemoryRegistersTakesOnlyOneRegister) {
    // FX55 stores V0 to VX, a second register would end up in the bits of the opcode that are fixed
    EXPECT_EXIT(Assemble(Program("    STMR V3 V1"), memory), ::testing::ExitedWithCode(EXIT_CODE_ASSEMBLER_ERROR), "");
}

TEST_F(AssemblerDeathTest, UnknownMnemonicIsAnError) {
    EXPECT_EXIT(Assemble(Program("    XYZ V0"), memory), ::testing::ExitedWithCode(EXIT_CODE_ASSEMBLER_ERROR), "");
}

} // namespace
