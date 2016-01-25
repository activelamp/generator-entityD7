<?php

/**
 * @file
 * Custom Entity type editing UI.
 */

/**
 * Generates the custom_entity type editing form.
 */
function custom_entity_type_form($form, &$form_state, $custom_entity_type, $op = 'edit') {

  if ($op == 'clone') {
    $custom_entity_type->label .= ' (cloned)';
    $custom_entity_type->type = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $custom_entity_type->label,
    '#description' => t('The human-readable name of this custom_entity type.'),
    '#required' => TRUE,
    '#size' => 30,
  );
  // Machine-readable type name.
  $form['type'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($custom_entity_type->type) ? $custom_entity_type->type : '',
    '#maxlength' => 32,
    '#machine_name' => array(
      'exists' => 'custom_entity_get_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this custom_entity type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save custom_entity type'),
    '#weight' => 40,
  );
  $form['weight'] = array(
    '#type' => 'weight',
    '#title' => t('Weight'),
    '#default_value' => $custom_entity_type->weight,
    '#description' => t('When showing custom_entity, those with lighter (smaller) weights get listed before custom_entity with heavier (larger) weights.'),
    '#weight' => 10,
  );

  if ($op != 'add' && $op != 'clone') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete custom_entity type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('custom_entity_type_form_submit_delete')
    );
  }
  return $form;
}

/**
 * Form API submit callback for the type form.
 */
function custom_entity_type_form_submit(&$form, &$form_state) {
  $custom_entity_type = entity_ui_form_submit_build_entity($form, $form_state);
  // Save and go back.
  $custom_entity_type->save();
  $form_state['redirect'] = 'admin/structure/entity/custom_entity';
}

/**
 * Form API submit callback for the delete button.
 */
function custom_entity_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/entity/custom_entity/manage/' . $form_state['custom_entity_type']->type . '/delete';
}