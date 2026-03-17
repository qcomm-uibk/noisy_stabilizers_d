# quantum-computing-service JSON format for quantum-circuits


## Summary

this is a short description of the JSON based format which is used to specify quantum-circuits for import into the quantum-computing service via its (REST) API

it is heavily based on the IonQ JSON format:

- [https://docs.ionq.com/](https://docs.ionq.com/)


## circuit

a circuit is specified by some meta-data - mainly the number of qubits and an optional name -, a certain JSON structure and an ordered list of operations.

### metadata

defined by:

* key "name" ... the value specifies a name for the circuit
* key "input" ... the quantum-circuit itself

the input keyword contains:

* key "qubit" ... the integer-value specifies the number of qubits used within the circuit
* key "circuit" ... the list of quantum-operations representing the circuit

### structure

the final structure looks like:

example
```
{
  \"name\": \"test circuit name\",
  \"input\": {
    \"qubits\": 3,
    \"circuit\": [

      <list of operations>

    ]
  }
}
```


## operations

currently supported operations are one of the following 3 types:

- unitary-operation
- partial-trace
- measurement

### unitary-operation

defined by:

- possess the key "gate"
- has one or more control qubits
- has one or more target qubits
- the qubits are specified either as an integer or a list of integers 
- the value for the gate-keyword specifies the unitary-operation
- it has to be one of the following
  - hadamard ~ "h"
  - pauli X ~ "x"
  - pauli Y ~ "y"
  - pauli Z ~ "z"
  - phase ~ "s"
  - controlled not ~ "cnot"
  - rotation X ~ "rx"
  - rotation Y ~ "ry"
  - rotation Z ~ "rz"

example:

```
{
  \"gate\": \"cnot\",
  \"control\": 0,
  \"target\": 1
},
```

example rotation with its angle-parameter theta:

```
{
  \"gate\": \"rx\",
  \"target\": 0,
  \"parameters\": [1.23]
},
```

or as a named parameter, with either key "theta" or "angle":

```
{
  \"gate\": \"ry\",
  \"target\": 0,
  \"parameters\": \"{\\\"theta\\\": 1.23}\"
},
```

### measurement

defined by:

- possess the key "measurement"
- has one or more measure qubits
- the qubits are specified either as an integer or a list of integers 
- the value for the measurement-keyword specifies the measurement-operation
- it has to be one of the following
  - pauli X ~ "x"
  - pauli Y ~ "y"
  - pauli Z ~ "z"
  - bell ~ "bell"


example:

```
{
  \"measurement\": \"bell\",
  \"measure\": [2, 3]
},
```

### partial-trace

defined by:

- possess the key "partialtrace"
- has one or more trace qubits
- the qubits are specified either as an integer or a list of integers 
- the value for the partialtrace-keyword specifies the name for the partial-trace operations

example:

```
{
  \"partialtrace\": \"a\",
  \"trace\": [1]
}
```


## quantum-channel

define a quantum-channel by using either an arbitrary set of kraus-operators specified by its matrices or a named noise-model.

defined by:

- possess the key "channel"
- the affected qubits are specified either as an integer or a list of integers

### arbitrary quantum-channels

define a quantum-channel by explicitly specifying the matrices of its kraus-operators:

- the value for the key "channel" is "arbitrary"
- possess the key "krausoperators"
- which contains a list of kraus-operators defined by its matrices
- the matrix is defined by its key "matrix" and a JSON-string defining its coefficients as its value
- the coefficients of this matrix can be complex-numbers - if you want to read about how to embed them into the JSON-data for our API, see the document [COMPLEXNUMBERS.md](COMPLEXNUMBERS.md).


arbitrary example:

```
{
  \"channel\": \"arbitrary\",
  \"qubits\": [0, 1, 2],
  \"krausoperators\": [
    {
      \"matrix\": \"[[1.0, 0.0], [0.0, 1.0]]\"
    },
    {
      \"matrix\": \"[[0.0, {\\\"real\\\": 0.0, \\\"imag\\\": -1.0}], [{\\\"real\\\": 0.0, \\\"imag\\\": 1.0}, 0.0]]\"
    },
    {
      \"matrix\": \"[[0.0, 1.0], [1.0, 0.0]]\"
    }
  ]
},

```

### named quantum-channels

the value for the key "channel" can be one of the following:

- bitflip ~ with an additional parameter "probability"
- phaseflip ~ with an additional parameter "probability"
- bitphaseflip ~ with an additional parameter "probability"
- depolarizing ~ with additional parameters "lambda" and "dimension"
- amplitude-damping ~ with an additional parameter "gamma"

named examples:

```
{
  \"channel\": \"bitphaseflip\",
  \"probability\": 0.5,
  \"qubits\": [0, 1]
},
{
  \"channel\": \"depolarizing\",
  \"lambda\": 1.23,
  \"dimension\": 2,
  \"qubits\": [1, 3]
},
{
  \"channel\": \"amplitude-damping\",
  \"gamma\": 0.23,
  \"qubits\": [4, 5]
},

```


## noisy-unitaries

define a noise-model for unitary-operations by using either an arbitrary set of kraus-operators specified by its matrices or a named noise-model.

the noisy-unitary is defined by:

- a unitary-operation which includes the key "noise"

### arbitrary noisy-unitary

define a noise-model for a unitary-operation by explicitly specifying the matrices of its kraus-operators:

- the key "noise" itself contains the key "channel" or "model" with the value "arbitrary"
- and contains the key "qubits" providing a list of integer which specify the target-qubits for the noise-model
- if no value(s) for key "qubits" are provided, the first target-qubit of the unitary-operation will be used
- and contains the key "krausoperators"
- the matrices are defined by their key "matrix" and a JSON-string defining its coefficients as its value
- the coefficients of these matrices can be complex-numbers - if you want to read about how to embed them into the JSON-data for our API, see the document [COMPLEXNUMBERS.md](COMPLEXNUMBERS.md).
- the affected qubits are the one/ones from the unitary-operation

arbitrary example:

```
{
  \"gate\": \"cnot\",
  \"control\": 0,
  \"target\": 1,
  \"noise\": {
    \"channel\": \"arbitrary\",
    \"qubits\": [0, 1],
    \"krausoperators\": [
      {
        \"matrix\": \"[[{\\\"real\\\": 0.5, \\\"imag\\\": 0.5}, 0.0], [0.0, {\\\"real\\\": 0.5, \\\"imag\\\": 0.5}]]\"
      },
      {
        \"matrix\": \"[[{\\\"real\\\": -0.5, \\\"imag\\\": -0.5}, 0.0], [0.0, {\\\"real\\\": 0.5, \\\"imag\\\": 0.5}]]\"
      }
    ]
  }
},
```

### named noisy-unitaries

the key "noise" contains a key "channel" or "model" and its value can be one of the following:

- bitflip ~ with an additional parameter "probability"
- phaseflip ~ with an additional parameter "probability"
- bitphaseflip ~ with an additional parameter "probability"
- depolarizing ~ with additional parameters "lambda" and "dimension"
- amplitude-damping ~ with an additional parameter "gamma"

named examples:

```
{
  \"gate\": \"cnot\",
  \"control\": 0,
  \"target\": 1,
  \"noise\": {
    \"channel\": \"bitflip\",
    \"probability\": 0.1,
    \"qubits\": [0, 1]
  }
},
{
  \"gate\": \"h\",
  \"target\": 2
  \"noise\": {
    \"channel\": \"depolarizing\",
    \"lambda\": 1.23,
    \"dimension\": 2,
    \"qubits\": [2]
},
{
  \"gate\": \"h\",
  \"target\": 3
  \"noise\": {
    \"model\": \"amplitude-damping\",
    \"gamma\": 0.42,
    \"qubits\": [3]
}
```


## noisy-measurements

define a noise-model for measurement-operations by using either an arbitrary set of kraus-operators specified by its matrices or a named noise-model.

the noisy-measurement is defined by:

- a measurement-operation which includes the key "noise"

### arbitrary noisy-measurement

define a noise-model for a measurement-operation by explicitly specifying the matrices of its kraus-operators:

- the key "noise" itself contains the key "channel" or "model" with the value "arbitrary"
- and contains the key "qubits" providing a list of integer which specify the target-qubits for the noise-model
- if no value(s) for key "qubits" are provided, the first target-qubit of the measurement-operation will be used
- and contains the key "krausoperators"
- the matrices are defined by their key "matrix" and a JSON-string defining its coefficients as its value
- the coefficients of these matrices can be complex-numbers - if you want to read about how to embed them into the JSON-data for our API, see the document [COMPLEXNUMBERS.md](COMPLEXNUMBERS.md).
- the affected qubits are the one/ones from the measurement-operation

arbitrary example:

```
{
  \"measurement\": \"bell\",
  \"measure\": [0, 1],
  \"noise\": {
    \"channel\": \"arbitrary\",
    \"qubits\": [0, 1],
    \"krausoperators\": [
      {
        \"matrix\": \"[[{\\\"real\\\": 0.5, \\\"imag\\\": 0.5}, 0.0], [0.0, {\\\"real\\\": 0.5, \\\"imag\\\": 0.5}]]\"
      },
      {
        \"matrix\": \"[[{\\\"real\\\": -0.5, \\\"imag\\\": -0.5}, 0.0], [0.0, {\\\"real\\\": 0.5, \\\"imag\\\": 0.5}]]\"
      }
    ]
  }
},
```

### named noisy-measurement

the key "noise" contains a key "channel" or "model" and its value can be one of the following:

- bitflip ~ with an additional parameter "probability"
- phaseflip ~ with an additional parameter "probability"
- bitphaseflip ~ with an additional parameter "probability"
- depolarizing ~ with additional parameters "lambda" and "dimension"
- amplitude-damping ~ with an additional parameter "gamma"

named examples:

```
{
  \"measurement\": \"bell\",
  \"measure\": [0, 1],
  \"noise\": [
    {
      \"channel\": \"amplitude-damping\",
      \"gamma\": 0.23,
      \"qubits\": [0, 1]
    }
  ]
},
{
  \"measurement\": \"bell\",
  \"measure\": [1, 2],
  \"noise\": [
    }
      \"model\": \"amplitude-damping\",
      \"gamma\": 0.42,
      \"qubits\": [1, 2]
    },
    }
      \"channel\": \"depolarizing\",
      \"lambda\": 1.23,
      \"dimension\": 1,
      \"qubits\": 2
    }
  ]
},
{
  \"measurement\": \"x\",
  \"measure\": [3],
  \"noise\": [
    {
      \"channel\": \"bitflip\",
      \"probability\": 0.13,
      \"qubits\": 3,
    },
    {
      \"model\": \"amplitude-damping\",
      \"gamma\": 0.34,
      \"qubits\": [3]
    }
  ]
}
```


## condition-operations

the possibility to define a list of conditions as a standalone quantum-operations referencing arbitrary measurements / already existing measurement-outcomes. the result is the optional execution of defined sub-circuits - list of operations - if a certain condition is matched.

the list of conditions is defined by:

- an operation which includes the key "measured-qubits"

### onoutcome

define conditions for a measurement-operations based on a set of matches against the actual measurement-outcome represented in its classical-bits:

- the key "onoutcome" contains the necessary measurement-outcome for the condition
- the key "circuit" contains a quantum-circuit - eg. list of operations - which should be executed, if the condition is met

example:

```
{
  \"measured-qubits\": [0, 1],
  \"conditions\": [
    {
      \"onoutcome\": 0,
      \"circuit\": [
        {
          \"gate\": \"x\",
          \"target\": 2,
          \"noise\": [
            {
              \"channel\": \"depolarizing\",
              \"lambda\": 0.955,
              \"qubits\": [2]
            }
          ]
        }
      ]
    },
    {
      \"onoutcome\": 1,
      \"circuit\": [
        {
          \"gate\": \"h\",
          \"target\": 2,
          \"noise\": [
            {
              \"model\": \"bitflip\",
              \"probability\": 0.2
            },
            {
              \"channel\": \"arbitrary\",
              \"krausoperators\": [
                {
                  \"matrix\": \"[[0.5773502691896258, 0.0], [0.0, 0.5773502691896258]]\"
                },
                {
                  \"matrix\": \"[[0.5773502691896258, 0.0], [0.0, 0.5773502691896258]]\"
                },
                {
                  \"matrix\": \"[[0.5773502691896258, 0.0], [0.0, 0.5773502691896258]]\"
                }
              ]
            }
          ]
        }
      ]
    },
  ]
}
```

## conditional-measurements

the possibility to define a list of conditions for measurement-operations, which result in the optional execution of defined sub-circuits - list of operations - if a certain condition is matched.

the list of conditions is defined by:

- a measurement-operation which includes the key "conditions"

### onoutcome

define conditions for a measurement-operations based on a set of matches against the actual measurement-outcome represented in its classical-bits:

- the key "onoutcome" contains the necessary measurement-outcome for the condition
- the key "circuit" contains a quantum-circuit - eg. list of operations - which should be executed, if the condition is met

example:

```
{
  \"measurement\": \"x\",
  \"measure\": 0,
  \"conditions\": [
    {
      \"onoutcome\": 0,
      \"circuit\": [
        {
          \"gate\": \"x\",
          \"target\": 2,
          \"noise\": [
            {
              \"channel\": \"depolarizing\",
              \"lambda\": 0.955,
              \"qubits\": [2]
            }
          ]
        }
      ]
    },
    {
      \"onoutcome\": 1,
      \"circuit\": [
        {
          \"gate\": \"h\",
          \"target\": 2,
          \"noise\": [
            {
              \"model\": \"bitflip\",
              \"probability\": 0.2
            },
            {
              \"channel\": \"arbitrary\",
              \"krausoperators\": [
                {
                  \"matrix\": \"[[0.5773502691896258, 0.0], [0.0, 0.5773502691896258]]\"
                },
                {
                  \"matrix\": \"[[0.5773502691896258, 0.0], [0.0, 0.5773502691896258]]\"
                },
                {
                  \"matrix\": \"[[0.5773502691896258, 0.0], [0.0, 0.5773502691896258]]\"
                }
              ]
            }
          ]
        }
      ]
    },
  ]
}
```


## full example

a full example using all possible types of operations:

```
{
  \"name\": \"test ghz circuit with quantum-channels and a noisy-unitary operation\",
  \"input\": {
    \"qubits\": 6,
    \"circuit\": [
      {
        \"gate\": \"h\",
        \"target\": 0,
        \"noise\": {
          \"channel\": \"arbitrary\",
          \"qubits\": [0],
          \"krausoperators\": [
            {
              \"matrix\": \"[[{\\\"real\\\": 0.5, \\\"imag\\\": 0.5}, 0.0], [0.0, {\\\"real\\\": 0.5, \\\"imag\\\": 0.5}]]\"
            },
            {
              \"matrix\": \"[[{\\\"real\\\": -0.5, \\\"imag\\\": -0.5}, 0.0], [0.0, {\\\"real\\\": 0.5, \\\"imag\\\": 0.5}]]\"
            }
          ]
        }
      },
      {
        \"gate\": \"cnot\",
        \"control\": 0,
        \"target\": 1
      },
      {
        \"gate\": \"rx\",
        \"target\": 0,
        \"parameters\": \"{\\\"theta\\\": 1.23}\"
      },
      {
        \"gate\": \"rx\",
        \"target\": 1,
        \"parameters\": [-1.23]
      },
      {
        \"gate\": \"cnot\",
        \"control\": 0,
        \"target\": 2,
        \"noise\": {
          \"model\": \"amplitude-damping\",
            \"gamma\": 0.12,
            \"qubits\": [0, 2]
        }
      },
      {
        \"gate\": \"h\",
        \"target\": 3
      },
      {
        \"gate\": \"cnot\",
        \"control\": 3,
        \"target\": 4
      },
      {
        \"gate\": \"cnot\",
        \"control\": 3,
        \"target\": 5
      },
      {
        \"channel\": \"bitphaseflip\",
        \"probability\": 0.5,
        \"qubits\": [0, 1]
      },
      {
        \"channel\": \"arbitrary\",
        \"qubits\": [2, 3],
        \"krausoperators\": [
          {
            \"matrix\": \"[[1.0, 0.0], [0.0, 1.0]]\"
          },
          {
            \"matrix\": \"[[0.0, {\\\"real\\\": 0.0, \\\"imag\\\": -1.0}], [{\\\"real\\\": 0.0, \\\"imag\\\": 1.0}, 0.0]]\"
          }
        ]
      },
      {
        \"measurement\": \"bell\",
        \"measure\": [2, 3],
        \"noise\": {
          \"channel\": \"arbitrary\",
          \"qubits\": [2, 3],
          \"krausoperators\": [
            {
              \"matrix\": \"[[{\\\"real\\\": 0.5, \\\"imag\\\": 0.5}, 0.0], [0.0, {\\\"real\\\": 0.5, \\\"imag\\\": 0.5}]]\"
            },
            {
              \"matrix\": \"[[{\\\"real\\\": -0.5, \\\"imag\\\": -0.5}, 0.0], [0.0, {\\\"real\\\": 0.5, \\\"imag\\\": 0.5}]]\"
            }
          ]
        }
        \"conditions\": [
          {
            \"onoutcome\": 1,
            \"circuit\": [
              {
                \"gate\": \"x\",
                \"target\": 0,
                \"noise\": [
                  {
                    \"model\": \"bitphaseflip\",
                    \"probability\": 0.2
                  },
                  {
                    \"model\": \"amplitude-damping\",
                    \"gamma\": 0.123,
                    \"qubits\": [0]
                  }
                ]
              }
            ]
          },
          {
            \"onoutcome\": 3,
            \"circuit\": [
              {
                \"gate\": \"z\",
                \"target\": 0,
                \"noise\": [
                  {
                    \"model\": \"phaseflip\",
                    \"probability\": 0.3
                  },
                  {
                    \"channel\": \"depolarizing\",
                    \"lambda\": 0.234,
                    \"qubits\": [0]
                  }
                ]
              }
            ]
          }
        ]
      },
      {
        \"measurement\": \"x\",
        \"measure\": [0],
        \"noise\": [
          {
            \"channel\": \"bitflip\",
            \"probability\": 0.13,
            \"qubits\": 0,
          },
          {
            \"model\": \"amplitude-damping\",
            \"gamma\": 0.34,
            \"qubits\": [0]
          }
        ]
      },
      {
        \"measured-qubits\": [0, 1],
        \"conditions\": [
          {
            \"onoutcome\": 0,
            \"circuit\": [
              {
                \"gate\": \"x\",
                \"target\": 2,
                \"noise\": [
                  {
                    \"channel\": \"depolarizing\",
                    \"lambda\": 0.955,
                    \"qubits\": [2]
                  }
                ]
              }
            ]
          },
          {
            \"onoutcome\": 1,
            \"circuit\": [
              {
                \"gate\": \"h\",
                \"target\": 2,
                \"noise\": [
                  {
                    \"model\": \"bitflip\",
                    \"probability\": 0.2
                  },
                  {
                    \"channel\": \"arbitrary\",
                    \"krausoperators\": [
                      {
                        \"matrix\": \"[[0.5773502691896258, 0.0], [0.0, 0.5773502691896258]]\"
                      },
                      {
                        \"matrix\": \"[[0.5773502691896258, 0.0], [0.0, 0.5773502691896258]]\"
                      },
                      {
                         \"matrix\": \"[[0.5773502691896258, 0.0], [0.0, 0.5773502691896258]]\"
                      }
                    ]
                  }
                ]
              }
            ]
          },
        ]
      },
      {
        \"partialtrace\": \"a\",
        \"trace\": [1]
      }
    ]
  }
}

```