from __future__ import annotations

import argparse
import json
import shutil
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, Sequence

import onnx


@dataclass(frozen=True)
class ModelConfigData:
    sampling_rate: float
    hop_size: int
    mel_bins: int
    window_size: int
    expected_inputs: Sequence[str]
    expected_outputs: Sequence[str]

    def to_json(self) -> dict:
        return {
            "sampling_rate": self.sampling_rate,
            "hop_size": self.hop_size,
            "mel_bins": self.mel_bins,
            "window_size": self.window_size,
            "expected_inputs": list(self.expected_inputs),
            "expected_outputs": list(self.expected_outputs),
        }


def parse_name_list(raw_value: str | None) -> list[str]:
    if not raw_value:
        return []
    return [item.strip() for item in raw_value.split(",") if item.strip()]


def load_onnx_model(model_path: Path) -> onnx.ModelProto:
    return onnx.load(model_path.as_posix())


def get_io_names(model: onnx.ModelProto) -> tuple[list[str], list[str]]:
    initializer_names = {initializer.name for initializer in model.graph.initializer}
    inputs = [
        value.name
        for value in model.graph.input
        if value.name and value.name not in initializer_names
    ]
    outputs = [value.name for value in model.graph.output if value.name]
    return inputs, outputs


def ensure_output_dir(output_dir: Path) -> None:
    output_dir.mkdir(parents=True, exist_ok=True)


def copy_model_to_output(model_path: Path, output_dir: Path) -> Path:
    destination = output_dir / "model.onnx"
    shutil.copy2(model_path, destination)
    return destination


def write_config(output_dir: Path, config: ModelConfigData) -> Path:
    config_path = output_dir / "config.json"
    with config_path.open("w", encoding="utf-8") as handle:
        json.dump(config.to_json(), handle, indent=2)
        handle.write("\n")
    return config_path


def add_base_export_args(parser: argparse.ArgumentParser) -> None:
    parser.add_argument("--model", required=True, type=Path, help="Path to the ONNX model.")
    parser.add_argument(
        "--output-dir",
        required=True,
        type=Path,
        help="Directory to write model.onnx and config.json.",
    )
    parser.add_argument("--sample-rate", required=True, type=float, help="Model sampling rate.")
    parser.add_argument("--hop-size", required=True, type=int, help="Hop size in samples.")
    parser.add_argument("--mel-bins", required=True, type=int, help="Number of mel bins.")
    parser.add_argument("--window-size", required=True, type=int, help="FFT/window size.")
    parser.add_argument(
        "--input-names",
        type=str,
        default=None,
        help="Comma-separated ONNX input names (defaults to model IO).",
    )
    parser.add_argument(
        "--output-names",
        type=str,
        default=None,
        help="Comma-separated ONNX output names (defaults to model IO).",
    )


def build_config_from_args(args: argparse.Namespace, onnx_inputs: Iterable[str], onnx_outputs: Iterable[str]) -> ModelConfigData:
    input_names = parse_name_list(args.input_names) or list(onnx_inputs)
    output_names = parse_name_list(args.output_names) or list(onnx_outputs)
    return ModelConfigData(
        sampling_rate=args.sample_rate,
        hop_size=args.hop_size,
        mel_bins=args.mel_bins,
        window_size=args.window_size,
        expected_inputs=input_names,
        expected_outputs=output_names,
    )


def require_onnx_model(model_path: Path) -> None:
    if model_path.suffix.lower() != ".onnx":
        raise SystemExit(f"Expected an ONNX model, got: {model_path}")

