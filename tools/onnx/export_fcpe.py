from __future__ import annotations

import argparse

from pathlib import Path

from _common import (
    add_base_export_args,
    build_config_from_args,
    copy_model_to_output,
    ensure_output_dir,
    get_io_names,
    load_onnx_model,
    require_onnx_model,
    write_config,
)


def main() -> None:
    parser = argparse.ArgumentParser(description="Package an FCPE ONNX model with config.json.")
    add_base_export_args(parser)
    args = parser.parse_args()

    require_onnx_model(args.model)
    ensure_output_dir(args.output_dir)

    model = load_onnx_model(args.model)
    inputs, outputs = get_io_names(model)

    copy_model_to_output(Path(args.model), Path(args.output_dir))
    config = build_config_from_args(args, inputs, outputs)
    write_config(Path(args.output_dir), config)


if __name__ == "__main__":
    main()
