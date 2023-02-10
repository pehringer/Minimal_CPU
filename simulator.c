#include"simulator.h"




//Control unit bitmasks.
#define LINE_READ_OPERAND_REGISTER_NOT 1
#define LINE_WRITE_OPERAND_REGISTER 2
#define LINE_READ_OPERAND_REGISTER 4
#define LINE_WRITE_TEMPORARY_REGISTER 8
#define LINE_READ_TEMPORARY_REGISTER 16
#define LINE_WRITE_PROGRAM_COUNTER_REGISTER 32
#define LINE_READ_PROGRAM_COUNTER_REGISTER 64
#define LINE_WRITE_ACCUMULATOR_REGISTER 128
#define LINE_WRITE_INSTRUCTION_REGISTER 256
#define LINE_INPUT_RIGHT 512
#define LINE_INPUT_LEFT 1024
#define LINE_CARRY_IN 2048
#define LINE_WRITE_MEMORY 4096
#define LINE_READ_MEMORY 8192

//Sign unit bitmasks.
#define LINE_POSITIVE 1
#define LINE_NEGATIVE 2
#define POSITIVE_BITS 2147483647
#define NEGATIVE_BITS 2147483648




static void simulate_system_clock_cycle(struct simulation *sim) {
  /******* Simulate Control Unit *******/
  static const uint32_t CONTROL_UNIT_STATE[3][14][8] = {
    {//     Clock Count / Accumulator Register (0)
     //   0     1    2     3     4     5     6     7
      {8512, 2632, 560, 8258, 2632,  560,    0,    0}, //LOAD_I
      {8512, 2632, 560, 8204,  530,    0,    0,    0}, //LOAD
      {8512, 2632, 560, 8264, 8210, 2632,  560,    0}, //LOAD_A
      {8512, 2632, 560,  524, 1026,  656,    0,    0}, //SWAP
      {8512, 2632, 560, 1548,  656,    0,    0,    0}, //ADD
      {8512, 2632, 560, 3593,  656,    0,    0,    0}, //SUB
      {8512, 2632, 560, 5124,    0,    0,    0,    0}, //SAVE
      {8512, 2632, 560, 8264, 5136, 2632,  560,    0}, //SAVE_A
      {8512, 2632, 560,  548,    0,    0,    0,    0}, //JUMP
      {8512, 2632, 560, 8264,  560,    0,    0,    0}, //JUMP_A
      {8512, 2632, 560, 8264,  560,    0,    0,    0}, //JUMP_Z
      {8512, 2632, 560, 2632,  560,    0,    0,    0}, //JUMP_P
      {8512, 2632, 560, 2632,  560,    0,    0,    0}, //JUMP_N
      {8512, 2632, 560,    0,    0,    0,    0,    0}  //HALT
    },
    {//     Clock Count / Accumulator Register (+)
     //   0     1    2     3     4     5     6     7
      {8512, 2632, 560, 8258, 2632,  560,    0,    0}, //LOAD_I
      {8512, 2632, 560, 8204,  530,    0,    0,    0}, //LOAD
      {8512, 2632, 560, 8264, 8210, 2632,  560,    0}, //LOAD_A
      {8512, 2632, 560,  524, 1026,  656,    0,    0}, //SWAP
      {8512, 2632, 560, 1548,  656,    0,    0,    0}, //ADD
      {8512, 2632, 560, 3593,  656,    0,    0,    0}, //SUB
      {8512, 2632, 560, 5124,    0,    0,    0,    0}, //SAVE
      {8512, 2632, 560, 8264, 5136, 2632,  560,    0}, //SAVE_A
      {8512, 2632, 560,  548,    0,    0,    0,    0}, //JUMP
      {8512, 2632, 560, 8264,  560,    0,    0,    0}, //JUMP_A
      {8512, 2632, 560, 2632,  560,    0,    0,    0}, //JUMP_Z
      {8512, 2632, 560, 8264,  560,    0,    0,    0}, //JUMP_P
      {8512, 2632, 560, 2632,  560,    0,    0,    0}, //JUMP_N
      {8512, 2632, 560,    0,    0,    0,    0,    0}  //HALT
    },
    {//     Clock Count / Accumulator Register (-)
     //   0     1    2     3     4     5     6     7
      {8512, 2632, 560, 8258, 2632,  560,    0,    0}, //LOAD_I
      {8512, 2632, 560, 8204,  530,    0,    0,    0}, //LOAD
      {8512, 2632, 560, 8264, 8210, 2632,  560,    0}, //LOAD_A
      {8512, 2632, 560,  524, 1026,  656,    0,    0}, //SWAP
      {8512, 2632, 560, 1548,  656,    0,    0,    0}, //ADD
      {8512, 2632, 560, 3593,  656,    0,    0,    0}, //SUB
      {8512, 2632, 560, 5124,    0,    0,    0,    0}, //SAVE
      {8512, 2632, 560, 8264, 5136, 2632,  560,    0}, //SAVE_A
      {8512, 2632, 560,  548,    0,    0,    0,    0}, //JUMP
      {8512, 2632, 560, 8264,  560,    0,    0,    0}, //JUMP_A
      {8512, 2632, 560, 2632,  560,    0,    0,    0}, //JUMP_Z
      {8512, 2632, 560, 2632,  560,    0,    0,    0}, //JUMP_P
      {8512, 2632, 560, 8264,  560,    0,    0,    0}, //JUMP_N
      {8512, 2632, 560,    0,    0,    0,    0,    0}  //HALT
    }
  };
  int opcode = sim->instruction_register;
  int clock = sim->clock_count % 8;
  int sign = sim->sign_unit_lines;
  sim->control_unit_lines = CONTROL_UNIT_STATE[sign][opcode][clock];
  /******* Simulate Register Reads *******/
  sim->address_bus = 0;
  if(sim->control_unit_lines & LINE_READ_PROGRAM_COUNTER_REGISTER) {
    sim->address_bus |= sim->program_counter_register;
  }
  if(sim->control_unit_lines & LINE_READ_TEMPORARY_REGISTER) {
    sim->address_bus |= sim->temporary_register;
  }
  if(sim->control_unit_lines & LINE_READ_OPERAND_REGISTER) {
    sim->address_bus |= sim->operand_register;
  }
  if(sim->control_unit_lines & LINE_READ_OPERAND_REGISTER_NOT) {
    sim->address_bus |= ~(sim->operand_register);
  }
  /******* Simulate Adder Unit *******/
  sim->data_bus = 0;
  if(sim->control_unit_lines & LINE_INPUT_LEFT) {
    sim->data_bus += sim->accumulator_register;
  }
  if(sim->control_unit_lines & LINE_INPUT_RIGHT) {
    sim->data_bus += sim->address_bus;
  }
  if(sim->control_unit_lines & LINE_CARRY_IN) {
    sim->data_bus += 1;
  }
  /******* Simulate Memory Unit *******/
  if(sim->control_unit_lines & LINE_READ_MEMORY) {
    if(sim->address_bus < ADDRESS_INPUT_REGISTER) {
      sim->data_bus |= sim->memory[sim->address_bus];
    }
    if(sim->address_bus == ADDRESS_INPUT_REGISTER) {
      sim->data_bus |= sim->input_register;
    }
    if(sim->address_bus == ADDRESS_OUTPUT_REGISTER) {
      sim->data_bus |= sim->output_register;
    }
  }
  if(sim->control_unit_lines & LINE_WRITE_MEMORY) {
    if(sim->address_bus < ADDRESS_INPUT_REGISTER) {
      sim->memory[sim->address_bus] = sim->data_bus;
    }
    if(sim->address_bus == ADDRESS_INPUT_REGISTER) {
      sim->input_register = sim->data_bus;
    }
    if(sim->address_bus == ADDRESS_OUTPUT_REGISTER) {
      sim->output_register = sim->data_bus;
    }
  }
  /******* Simulate Register Writes *******/
  if(sim->control_unit_lines & LINE_WRITE_INSTRUCTION_REGISTER) {
    sim->instruction_register = sim->data_bus;
  }
  if(sim->control_unit_lines & LINE_WRITE_ACCUMULATOR_REGISTER) {
    sim->accumulator_register = sim->data_bus;
    sim->sign_unit_lines = 0;
    if(sim->accumulator_register & POSITIVE_BITS) {
      sim->sign_unit_lines = LINE_POSITIVE;
    }
    if(sim->accumulator_register & NEGATIVE_BITS) {
      sim->sign_unit_lines = LINE_NEGATIVE;
    }
  }
  if(sim->control_unit_lines & LINE_WRITE_PROGRAM_COUNTER_REGISTER) {
    sim->program_counter_register = sim->data_bus;
  }
  if(sim->control_unit_lines & LINE_WRITE_TEMPORARY_REGISTER) {
    sim->temporary_register = sim->data_bus;
  }
  if(sim->control_unit_lines & LINE_WRITE_OPERAND_REGISTER) {
    sim->operand_register = sim->data_bus;
  }
}

static void simulate_peripherals_clock_cycle(struct simulation *sim) {
  /******* Simulate Input Unit *******/
  if(sim->input_register == EMPTY) {
    sim->input_register = getc(stdin);
  }
  /******* Simulate Register Writes *******/
  if(sim->output_register != EMPTY) {
    putc(sim->output_register, stdout);
    fflush(stdout);
    sim->output_register = EMPTY;
  }
}

int simulate_clock_cycle(struct simulation *sim) {
  if(sim->instruction_register == HALT) {
    return 0;
  }
  simulate_system_clock_cycle(sim);
  simulate_peripherals_clock_cycle(sim);
  sim->clock_count++;
  return 1;
}




struct simulation* new_simulation(const char *filepath) {
  struct simulation *sim = malloc(sizeof(struct simulation));
  /******* Init Clock *******/
  sim->clock_count = 0;
  /******* Init Buses and Lines *******/
  sim->sign_unit_lines = 0;
  sim->control_unit_lines = 0;
  sim->data_bus = 0;
  sim->address_bus = 0;
  /******* Init Registers *******/
  sim->instruction_register = 0;
  sim->accumulator_register = 0;
  sim->program_counter_register = 0;
  sim->temporary_register = 0;
  sim->operand_register = 0;
  /******* Init Memory *******/
  sim->input_register = END_OF_INPUT;
  sim->output_register = EMPTY;
  struct binary bin = load_binary(filepath);
  for(int address = 0; address < bin.size; address++) {
    sim->memory[address] = bin.data[address];
  }
  free_binary(bin);
  return sim;
}

void delete_simulation(struct simulation *sim) {
  free(sim);
}




int main() {
  recreate_example_binaries();
  struct simulation *sim = new_simulation("binary_examples/test.BIN");
  while(simulate_clock_cycle(sim));
  delete_simulation(sim);
  return 0;
}















