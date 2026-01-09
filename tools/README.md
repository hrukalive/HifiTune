# Tools

## ONNX packaging and validation

The scripts in `tools/onnx/` package ONNX models with a `config.json` that the app
loads via `ModelConfig` (sampling rate, hop size, mel bins, window size, and expected
input/output names). They also include a validator to sanity-check I/O shapes and
sampling rate values.

### Requirements

Install Python packages before running:

```bash
pip install onnx onnxruntime
```

### Export RMVPE

```bash
python tools/onnx/export_rmvpe.py \
  --model /path/to/rmvpe.onnx \
  --output-dir /path/to/output/rmvpe \
  --sample-rate 16000 \
  --hop-size 160 \
  --mel-bins 0 \
  --window-size 1024
```

### Export FCPE

```bash
python tools/onnx/export_fcpe.py \
  --model /path/to/fcpe.onnx \
  --output-dir /path/to/output/fcpe \
  --sample-rate 44100 \
  --hop-size 512 \
  --mel-bins 80 \
  --window-size 2048
```

### Export NSF-HiFiGAN

```bash
python tools/onnx/export_nsfhifigan.py \
  --model /path/to/nsf_hifigan.onnx \
  --output-dir /path/to/output/nsf_hifigan \
  --sample-rate 44100 \
  --hop-size 512 \
  --mel-bins 80 \
  --window-size 2048
```

### Validate a packaged model

```bash
python tools/onnx/validate_model.py \
  --model /path/to/output/model.onnx \
  --config /path/to/output/config.json \
  --model-type rmvpe \
  --expected-sample-rate 16000
```

The validator checks:

- Expected input/output names from `config.json` exist in the ONNX model.
- Input/output tensor ranks align with the model type.
- `sampling_rate` matches `--expected-sample-rate` when provided.
