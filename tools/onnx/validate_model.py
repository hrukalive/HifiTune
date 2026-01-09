from __future__ import annotations

import argparse
import json
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable

import onnxruntime as ort


@dataclass(frozen=True)
class ValidationIssue:
    message: str


def load_config(config_path: Path) -> dict:
    with config_path.open("r", encoding="utf-8") as handle:
        return json.load(handle)


def to_name_map(items: Iterable[ort.NodeArg]) -> dict[str, ort.NodeArg]:
    return {item.name: item for item in items}


def validate_expected_names(config: dict, inputs: dict[str, ort.NodeArg], outputs: dict[str, ort.NodeArg]) -> list[ValidationIssue]:
    issues: list[ValidationIssue] = []
    expected_inputs = config.get("expected_inputs") or []
    expected_outputs = config.get("expected_outputs") or []

    for name in expected_inputs:
        if name not in inputs:
            issues.append(ValidationIssue(f"Missing expected input name: {name}"))

    for name in expected_outputs:
        if name not in outputs:
            issues.append(ValidationIssue(f"Missing expected output name: {name}"))

    return issues


def validate_rank(name: str, node: ort.NodeArg, expected_rank: int) -> list[ValidationIssue]:
    shape = node.shape
    if shape is None:
        return []
    if len(shape) != expected_rank:
        return [ValidationIssue(f"{name} has rank {len(shape)} (expected {expected_rank})")]
    return []


def validate_sample_rate(config: dict, expected_sample_rate: float | None) -> list[ValidationIssue]:
    issues: list[ValidationIssue] = []
    if expected_sample_rate is None:
        return issues

    config_rate = config.get("sampling_rate")
    if config_rate is None:
        issues.append(ValidationIssue("Config missing sampling_rate"))
        return issues

    if float(config_rate) != float(expected_sample_rate):
        issues.append(
            ValidationIssue(
                f"sampling_rate {config_rate} does not match expected {expected_sample_rate}"
            )
        )
    return issues


def validate_rmvpe(inputs: dict[str, ort.NodeArg], outputs: dict[str, ort.NodeArg]) -> list[ValidationIssue]:
    issues: list[ValidationIssue] = []
    if not inputs:
        return [ValidationIssue("RMVPE model has no inputs")]

    first_input = next(iter(inputs.values()))
    issues.extend(validate_rank("rmvpe input", first_input, 3))

    if not outputs:
        issues.append(ValidationIssue("RMVPE model has no outputs"))
    return issues


def validate_fcpe(inputs: dict[str, ort.NodeArg], outputs: dict[str, ort.NodeArg]) -> list[ValidationIssue]:
    issues: list[ValidationIssue] = []
    if not inputs:
        return [ValidationIssue("FCPE model has no inputs")]

    first_input = next(iter(inputs.values()))
    issues.extend(validate_rank("fcpe input", first_input, 3))

    if not outputs:
        issues.append(ValidationIssue("FCPE model has no outputs"))
    return issues


def validate_nsfhifigan(inputs: dict[str, ort.NodeArg], outputs: dict[str, ort.NodeArg]) -> list[ValidationIssue]:
    issues: list[ValidationIssue] = []
    if len(inputs) < 2:
        issues.append(ValidationIssue("NSF-HiFiGAN model should expose at least two inputs"))
        return issues

    for name, node in inputs.items():
        if "mel" in name.lower() or "f0" in name.lower() or "pitch" in name.lower():
            issues.extend(validate_rank(f"nsfhifigan input {name}", node, 3))

    if not outputs:
        issues.append(ValidationIssue("NSF-HiFiGAN model has no outputs"))
    return issues


def main() -> None:
    parser = argparse.ArgumentParser(description="Validate ONNX model IO against config.json.")
    parser.add_argument("--model", required=True, type=Path, help="Path to model.onnx")
    parser.add_argument("--config", required=True, type=Path, help="Path to config.json")
    parser.add_argument(
        "--model-type",
        required=True,
        choices=("rmvpe", "fcpe", "nsfhifigan"),
        help="Model type to validate.",
    )
    parser.add_argument(
        "--expected-sample-rate",
        type=float,
        default=None,
        help="Expected sampling rate to compare against config.json.",
    )

    args = parser.parse_args()

    config = load_config(args.config)
    session = ort.InferenceSession(args.model.as_posix(), providers=["CPUExecutionProvider"])
    inputs = to_name_map(session.get_inputs())
    outputs = to_name_map(session.get_outputs())

    issues: list[ValidationIssue] = []
    issues.extend(validate_expected_names(config, inputs, outputs))
    issues.extend(validate_sample_rate(config, args.expected_sample_rate))

    if args.model_type == "rmvpe":
        issues.extend(validate_rmvpe(inputs, outputs))
    elif args.model_type == "fcpe":
        issues.extend(validate_fcpe(inputs, outputs))
    else:
        issues.extend(validate_nsfhifigan(inputs, outputs))

    if issues:
        for issue in issues:
            print(f"ERROR: {issue.message}")
        raise SystemExit(1)

    print("Validation passed.")


if __name__ == "__main__":
    main()
