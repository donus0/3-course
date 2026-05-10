"""Математический слой: разбор данных и статистические вычисления."""

from matstat_app.math.compute import (
    AnalysisResult,
    IntervalBin,
    analyze,
    assign_bins,
    build_intervals,
    chi2_normality_gof,
    effective_start_index0,
    mean_test_zero,
    parse_rows,
    sample_row_numbers_1based,
    select_sample,
    shapiro_wilk_normal_hint,
    variance_test_sigma2,
)
from matstat_app.math.dataset import DEFAULT_DATASET_TEXT

__all__ = [
    "AnalysisResult",
    "DEFAULT_DATASET_TEXT",
    "IntervalBin",
    "analyze",
    "assign_bins",
    "build_intervals",
    "chi2_normality_gof",
    "effective_start_index0",
    "mean_test_zero",
    "parse_rows",
    "sample_row_numbers_1based",
    "select_sample",
    "shapiro_wilk_normal_hint",
    "variance_test_sigma2",
]
