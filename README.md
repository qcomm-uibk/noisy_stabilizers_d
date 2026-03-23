# noisy_stabilizers_d
This is a library that implements the Noisy Stabilizer Formalism for qudits in prime dimensions.

This formalism was introduced in:

> Noisy stabilizer formalism <br>
> M. F. Mor-Ruiz, W. Dür <br>
> Phys. Rev. A **107**, 032424 (2023); DOI: [10.1103/PhysRevA.107.032424](https://doi.org/10.1103/PhysRevA.107.032424) <br>
> Preprint: [arXiv:2212.08677 [quant-ph]](https://doi.org/10.48550/arXiv.2212.08677)

> Qudit Noisy Stabilizer Formalism <br>
> P. Aigner, M. F. Mor-Ruiz, W. Dür <br>
> Phys. Rev. A **112**, 022402 (2025); DOI: [10.1103/gqfw-x72s](https://doi.org/10.1103/gqfw-x72s) <br>
> Preprint: [arXiv:2505.03889 [quant-ph]](https://doi.org/10.48550/arXiv.2505.03889)

The main insight of this approach is that the
noise channels can be tracked individually instead
of being combined to one global channel,
e.g. local depolarizing noise on every qubit is highly
structured, but nonetheless a full rank noise channel
viewed in a global picture.
