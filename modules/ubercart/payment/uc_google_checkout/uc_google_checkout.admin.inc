<?php
// $Id: uc_google_checkout.admin.inc,v 1.15 2010/08/25 13:41:20 islandusurper Exp $

/**
 * @file
 * Google Checkout administration menu items.
 *
 */

function uc_google_checkout_settings($form, &$form_state) {
  $form['authentication'] = array(
    '#type' => 'fieldset',
    '#title' => t('Authentication'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['authentication']['uc_google_checkout_merchant_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Merchant ID'),
    '#default_value' => variable_get('uc_google_checkout_merchant_id', ''),
  );
  $form['authentication']['uc_google_checkout_merchant_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Merchant key'),
    '#default_value' => variable_get('uc_google_checkout_merchant_key', ''),
    '#description' => t('Used to sign cart information. Keep it secret, keep it safe.'),
  );

  $form['authentication']['sandbox'] = array(
    '#type' => 'fieldset',
    '#title' => t('Test Environment settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['authentication']['sandbox']['uc_google_checkout_mode'] = array(
    '#type' => 'radios',
    '#title' => t('Test Mode'),
    '#default_value' => variable_get('uc_google_checkout_mode', 'checkout'),
    '#options' => array('sandbox' => t('On'), 'checkout' => t('Off')),
  );
  $form['authentication']['sandbox']['uc_google_checkout_test_merchant_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Test Merchant ID'),
    '#description' => t('Only needed for test mode. Click <a href="http://code.google.com/apis/checkout/developer/index.html#integration_overview" target="_blank">here</a> for more info.'),
    '#default_value' => variable_get('uc_google_checkout_test_merchant_id', ''),
    '#required' => FALSE,
  );
  $form['authentication']['sandbox']['uc_google_checkout_test_merchant_key'] = array(
    '#type' => 'textfield',
    '#title' => t('Test Merchant Key'),
    '#default_value' => variable_get('uc_google_checkout_test_merchant_key', ''),
    '#required' => FALSE,
  );

  $form['messages'] = array(
    '#type' => 'fieldset',
    '#title' => t('Customer messages'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['messages']['uc_google_checkout_order_cancel_reason'] = array(
    '#type' => 'textarea',
    '#title' => t('Reason for canceling order'),
    '#description' => t('This message will be sent with the cancelation notice through Google Checkout. Any comment given when the order is canceled will be sent as a separate message. This message uses <a href="!url">site, store, and order tokens</a>.', array('!url' => 'admin/store/help/tokens')),
    '#default_value' => variable_get('uc_google_checkout_order_cancel_reason', t('Order canceled. See order comments at [uc_order:url] for more information.')),
  );

  $form['button'] = array(
    '#type' => 'fieldset',
    '#title' => t('Button settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['button']['uc_google_checkout_button_color'] = array(
    '#type' => 'radios',
    '#title' => t('Button background color'),
    '#default_value' => variable_get('uc_google_checkout_button_color', 'trans'),
    '#options' => array('trans' => t('Transparent'), 'white' => t('White')),
  );
  $form['button']['uc_google_checkout_button_size'] = array(
    '#type' => 'select',
    '#title' => t('Button size'),
    '#options' => array('large' => t('Large'), 'medium' => t('Medium'), 'small' => t('Small')),
    '#default_value' => variable_get('uc_google_checkout_button_size', 'large'),
  );
  $form['button']['uc_google_checkout_button_align'] = array(
    '#type' => 'radios',
    '#title' => t('Button alignment'),
    '#default_value' => variable_get('uc_google_checkout_button_align', 'right'),
    '#options' => array('right' => t('Right'), 'left' => t('Left')),
  );

  return system_settings_form($form);
}

function uc_google_checkout_shipping_settings($form, &$form_state) {
  $form['#tree'] = TRUE;

  $services = uc_google_checkout_shipping_services();

  foreach ($services as $key => $title) {
    $shipping_settings = variable_get('uc_google_checkout_shipping_' . $key, array());
    $form['services'][$key]['enable'] = array(
      '#type' => 'checkbox',
      '#default_value' => $shipping_settings['enable'],
    );
    $form['services'][$key]['title'] = array(
      '#markup' => $title,
    );
    $form['services'][$key]['default'] = array(
      '#type' => 'textfield',
      '#default_value' => $shipping_settings['default'],
      '#size' => 10,
      '#field_prefix' => '$',
    );
    $form['services'][$key]['pickup'] = array(
      '#type' => 'select',
      '#default_value' => $shipping_settings['pickup'],
      '#options' => array(
        'REGULAR_PICKUP' => t('Regular pickup'),
        'SPECIAL_PICKUP' => t('Special pickup'),
        'DROP_OFF' => t('Drop off'),
      ),
    );
    $form['services'][$key]['fixed_charge'] = array(
      '#type' => 'textfield',
      '#default_value' => $shipping_settings['fixed_charge'],
      '#size' => 10,
      '#field_prefix' => '$',
    );
    $form['services'][$key]['percent_adjustment'] = array(
      '#type' => 'textfield',
      '#default_value' => $shipping_settings['percent_adjustment'],
      '#size' => 5,
      '#field_suffix' => t('%'),
    );
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}

function theme_uc_google_checkout_shipping_settings($variables) {
  $form = $variables['form'];

  $header = array(t('Enable'), t('Service'), t('Default price'), t('Pickup type'), t('Handling charge'), t('Percentage markup/discount'));

  $companies = uc_google_checkout_shipping_companies();
  $company = '';
  $rows = array();
  foreach (element_children($form['services']) as $key) {
    $row = array();
    $temp = explode('_', $key, 2);
    if ($company != $temp[0]) {
      $company = $temp[0];
      $rows[] = array(array('data' => '<strong>' . $companies[$company] . '</strong>', 'colspan' => 6));
    }
    foreach (array('enable', 'title', 'default', 'pickup', 'fixed_charge', 'percent_adjustment') as $field) {
      $row[] = drupal_render($form['services'][$key][$field]);
    }
    $rows[] = $row;
  }
  $output = theme('table', array('header' => $header, 'rows' => $rows)) . drupal_render_children($form);

  return $output;
}

function uc_google_checkout_shipping_settings_validate($form, &$form_state) {
  foreach ($form_state['values']['services'] as $key => $service) {
    if ($service['enable'] && empty($service['default'])) {
      form_set_error('services][' . $key . '][default', t('A default shipping cost is required.'));
    }
    if (!empty($service['default']) && (!is_numeric($service['default']) || $service['default'] < 0)) {
      form_set_error('services][' . $key . '][default', t('The default shipping cost should be a positive number.'));
    }
    if (!empty($service['fixed_charge']) && !is_numeric($service['fixed_charge'])) {
      form_set_error('services][' . $key . '][fixed_charge', t('The fixed charge should be a number.'));
    }
    if (!empty($service['percent_adjustment']) && !is_numeric($service['percent_adjustment'])) {
      form_set_error('services][' . $key . '][percent_adjustment', t('The percent adjustment should be a number.'));
    }
  }
}

function uc_google_checkout_shipping_settings_submit($form, &$form_state) {
  foreach ($form_state['values']['services'] as $key => $service) {
    variable_set('uc_google_checkout_shipping_' . $key, $service);
  }
  drupal_set_message(t('The configuration options have been saved.'));
}

function uc_google_checkout_taxes_settings($form, &$form_state) {
  $form['#tree'] = TRUE;

  $result = db_query("SELECT zone, rate, tax_shipping FROM {uc_gc_taxes} ORDER BY zone");
  foreach ($result as $tax) {
    $form['taxes'][$tax->zone]['delete'] = array(
      '#type' => 'checkbox',
      '#default_value' => FALSE,
    );
    $form['taxes'][$tax->zone]['zone'] = array(
      '#type' => 'value',
      '#value' => $tax->zone,
    );
    $form['taxes'][$tax->zone]['rate'] = array(
      '#type' => 'textfield',
      '#default_value' => $tax->rate,
      '#size' => 8
    );
    $form['taxes'][$tax->zone]['tax_shipping'] = array(
      '#type' => 'checkbox',
      '#default_value' => $tax->tax_shipping,
    );
  }

  $form['taxes']['new']['delete'] = array(
    '#type' => 'checkbox',
    '#disabled' => TRUE,
  );
  $form['taxes']['new']['zone'] = uc_zone_select('');
  $form['taxes']['new']['rate'] = array(
    '#type' => 'textfield',
    '#default_value' => '',
    '#size' => 8
  );
  $form['taxes']['new']['tax_shipping'] = array(
    '#type' => 'checkbox',
    '#default_value' => FALSE,
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save taxes'),
  );

  return $form;
}

function theme_uc_google_checkout_taxes_settings($variables) {
  $form = $variables['form'];

  $header = array(t('Delete'), uc_get_field_name('zone'), t('Rate'), t('Tax shipping?'));

  $rows = array();
  foreach (element_children($form['taxes']) as $zone) {
    $row = array();
    $row[] = drupal_render($form['taxes'][$zone]['delete']);
    if ($zone == 'new') {
      $row[] = drupal_render($form['taxes'][$zone]['zone']);
    }
    else {
      $row[] = $form['taxes'][$zone]['zone']['#value'];
    }
    $row[] = drupal_render($form['taxes'][$zone]['rate']);
    $row[] = drupal_render($form['taxes'][$zone]['tax_shipping']);
    $rows[] = $row;
  }

  $output = theme('table', array('header' => $header, 'rows' => $rows));
  $output .= drupal_render_children($form);

  return $output;
}

function uc_google_checkout_taxes_settings_submit($form, &$form_state) {
  foreach ($form_state['values']['taxes'] as $zone => $tax_values) {
    if ($zone == 'new' && $tax_values['zone'] && $tax_values['rate']) {
      db_merge('uc_gc_taxes')
        ->key(array('zone' => uc_get_zone_code($tax_values['zone'])))
        ->fields(array(
          'rate' => $tax_values['rate'],
          'tax_shipping' => $tax_values['tax_shipping'],
        ))
        ->execute();
    }
    elseif ($tax_values['delete']) {
      db_delete('uc_gc_taxes')
        ->condition('zone', $tax_values['zone'])
        ->execute();
    }
    else {
      db_update('uc_gc_taxes')
        ->fields(array(
          'rate' => $tax_values['rate'],
          'tax_shipping' => $tax_values['tax_shipping'],
        ))
        ->condition('zone', $tax_values['zone'])
        ->execute();
    }
  }
}

function uc_google_checkout_terminal($order) {
  $order_id = $order->order_id;

  if ($order === FALSE) {
    drupal_set_message(t('Order @order_id does not exist.', array('@order_id' => $order_id)));
    drupal_goto('admin/store/orders');
  }

  $build['return'] = array(
    '#markup' => l(t('Return to order view screen.'), 'admin/store/orders/' . $order_id),
  );

  $gc_balance = $order->gc_total;
  $payments = uc_payment_load_payments($order_id);
  if (is_array($payments)) {
    foreach ($payments as $payment) {
      if ($payment->method == 'Google Checkout') {
        $gc_balance -= $payment->amount;
      }
    }
  }
  $balance = uc_payment_balance($order);

  $build['instructions'] = array('#markup' => '<p>' . t('Use this terminal to process credit card payments:') . '</p>');

  $build['details'] = array(
    '#prefix' => '<table style="width: auto;"><tbody style="border-top: 0px;">',
    '#suffix' => '</tbody></table>',
  );

  $build['details']['order_total'] = array(
    '#prefix' => '<tr>',
    '#suffix' => '</tr>',
  );
  $build['details']['order_total']['label'] = array('#markup' => '<strong>' . t('Order total:') . '</strong>');
  $build['details']['order_total']['value'] = array('#theme' => 'uc_price', '#price' => $order->order_total);

  $build['details']['balance'] = array(
    '#prefix' => '<tr>',
    '#suffix' => '</tr>',
  );
  $build['details']['balance']['label'] = array('#markup' => '<strong>' . t('Balance:') . '</strong>');
  $build['details']['balance']['value'] = array('#theme' => 'uc_price', '#price' => $balance);

  $build['details']['gco_total'] = array(
    '#prefix' => '<tr>',
    '#suffix' => '</tr>',
  );
  $build['details']['gco_total']['label'] = array('#markup' => '<strong>' . t('Google Checkout total:') . '</strong>');
  $build['details']['gco_total']['value'] = array('#theme' => 'uc_price', '#price' => $order->gc_total);

  $build['details']['gco_balance'] = array(
    '#prefix' => '<tr>',
    '#suffix' => '</tr>',
  );
  $build['details']['gco_balance']['label'] = array('#markup' => '<strong>' . t('Google Checkout balance:') . '</strong>');
  $build['details']['gco_balance']['value'] = array('#theme' => 'uc_price', '#price' => $gc_balance);

  if (in_array($order->financial_state, array('REVIEWING', 'CHARGEABLE', 'CHARGED'))) {
    $build['form'] = drupal_get_form('uc_google_checkout_terminal_form', $order, $gc_balance);
  }

  return $build;
}

function uc_google_checkout_terminal_form($form, &$form_state, $order, $amount = 0) {
  if ($order->financial_state == 'CHARGED') {
    $form['action'] = array(
      '#type' => 'select',
      '#title' => t('Action'),
      '#default_value' => 'charge',
      '#options' => array(
        'charge' => t('Charge'),
        'refund' => t('Refund'),
      ),
    );
    $form['reason'] = array(
      '#type' => 'textfield',
      '#title' => t('Reason for refund'),
      '#weight' => 1,
    );
    $form['comment'] = array(
      '#type' => 'textarea',
      '#title' => t('Refund Comment'),
      '#weight' => 2,
    );
  }
  else {
    $form['action'] = array(
      '#type' => 'value',
      '#value' => 'charge',
    );
  }

  $form['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Amount'),
    '#default_value' => uc_currency_format($amount, FALSE, FALSE, '.'),
    '#size' => 10,
    '#weight' => 0,
    '#field_prefix' => variable_get('uc_sign_after_amount', FALSE) ? '' : variable_get('uc_currency_sign', '$'),
    '#field_suffix' => variable_get('uc_sign_after_amount', FALSE) ? variable_get('uc_currency_sign', '$') : '',
  );
  $form['gc_balance'] = array(
    '#type' => 'value',
    '#value' => $amount,
  );
  $form['gc_total'] = array(
    '#type' => 'value',
    '#value' => $order->gc_total,
  );
  $form['order_id'] = array(
    '#type' => 'hidden',
    '#value' => $order->order_id,
  );
  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#weight' => 10,
  );

  return $form;
}

function uc_google_checkout_terminal_form_validate($form, &$form_state) {
  if (!is_numeric($form_state['values']['amount']) || $form_state['values']['amount'] == 0) {
    form_set_error('amount', t('You must enter a number for the amount.'));
  }
  if ($form_state['values']['action'] == 'charge' && $form_state['values']['amount'] > $form_state['values']['gc_balance']) {
    form_set_error('amount', t('Google does not allow charges greater than the balance.'));
  }
  if ($form_state['values']['action'] == 'refund' && $form_state['values']['amount'] > $form_state['values']['gc_total'] - $form_state['values']['gc_balance']) {
    form_set_error('amount', t('Google does not allow refunds greater than the amount already charged.'));
  }
  if ($form_state['values']['action'] == 'refund' && empty($form_state['values']['reason'])) {
    form_set_error('reason', t('A reason for refunding the customer is required.'));
  }

  $order = uc_order_load($form_state['values']['order_id']);
  if ($order === FALSE) {
    form_set_error('', t('Invalid order ID.  Unable to process payment.'));
  }
}

function uc_google_checkout_terminal_form_submit($form, &$form_state) {
  if ($form_state['values']['action'] == 'charge') {
    $form_state['redirect'] = uc_google_checkout_charge($form_state['values']['order_id'], $form_state['values']['amount']);
  }
  elseif ($form_state['values']['action'] == 'refund') {
    $form_state['redirect'] = uc_google_checkout_refund($form_state['values']['order_id'], $form_state['values']['amount'], $form_state['values']['reason'], $form_state['values']['comment']);
  }
}
