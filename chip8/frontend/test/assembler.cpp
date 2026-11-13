#include <gtest/gtest.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <vector>

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
        MnemonicCase{"STK V4", 0xF40A}, MnemonicCase{"STBC V5", 0xF533}, MnemonicCase{"TGS", 0x00E1},
        MnemonicCase{"CLS", 0x00E0}, MnemonicCase{"RET", 0x00EE}, MnemonicCase{"EXT", 0x0002},
        MnemonicCase{"JMP 0x234", 0x1234}, MnemonicCase{"CAL 0x300", 0x2300}),
    [](::testing::TestParamInfo<MnemonicCase> const & info) { return "Case" + std::to_string(info.index); });

TEST_F(AssemblerTest, CommentsAndEmptyLinesAreIgnored) {
    Assemble(Program("# comment\n\n    MOV V0 0x01   # trailing comment\n\n    MOV V1 V0\n"), memory);
    EXPECT_EQ(0x6001, Opcode(memory, 0));
    EXPECT_EQ(0x8100, Opcode(memory, 1));
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

TEST_F(AssemblerDeathTest, UnknownMnemonicIsAnError) {
    EXPECT_EXIT(Assemble(Program("    XYZ V0"), memory), ::testing::ExitedWithCode(EXIT_CODE_ASSEMBLER_ERROR), "");
}

} // namespace
